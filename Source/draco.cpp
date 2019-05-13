#include "Helper.h"

#define DRACO_ANS_IO_BASE 256

struct rans_sym {
    uint32_t prob;
    uint32_t cum_prob;  // not-inclusive.
};

struct ProbabilityLess {
    explicit ProbabilityLess(const std::vector<rans_sym> *probs)
            : probabilities(probs) {}

    bool operator()(int i, int j) const {
        return probabilities->at(i).prob < probabilities->at(j).prob;
    }

    const std::vector<rans_sym> *probabilities;
};

struct AnsCoder {
    AnsCoder() : buf(nullptr), buf_offset(0), state(0) {}

    uint8_t *buf;
    int buf_offset;
    uint32_t state;
};

// Class for performing rANS encoding using a desired number of precision bits.
// The max number of precision bits is currently 19. The actual number of
// symbols in the input alphabet should be (much) smaller than that, otherwise
// the compression rate may suffer.
template<int rans_precision_bits_t>
class RAnsEncoder {
public:
    RAnsEncoder() {}

    // Provides the input buffer where the data is going to be stored.
    inline void write_init(uint8_t *const buf) {
        ans_.buf = buf;
        ans_.buf_offset = 0;
        ans_.state = l_rans_base;
    }

    // Needs to be called after all symbols are encoded.
    inline int write_end() {
        uint32_t state;
        state = ans_.state - l_rans_base;
    }

    // rANS with normalization.
    // sym->prob takes the place of l_s from the paper.
    // rans_precision is m.
    inline void rans_write(const struct rans_sym *const sym) {
        const uint32_t p = sym->prob;
        while (ans_.state >= l_rans_base / rans_precision * DRACO_ANS_IO_BASE * p) {
            ans_.buf[ans_.buf_offset++] = ans_.state % DRACO_ANS_IO_BASE;
            ans_.state /= DRACO_ANS_IO_BASE;
        }
        // TODO(ostava): The division and multiplication should be optimized.
        ans_.state =
                (ans_.state / p) * rans_precision + ans_.state % p + sym->cum_prob;
    }

private:
    static constexpr int rans_precision = 1 << rans_precision_bits_t;
    static constexpr int l_rans_base = rans_precision * 4;
    AnsCoder ans_;
};

bool Create(const uint64_t *frequencies, int num_symbols) {
    std::vector<rans_sym> probability_table_;
    int rans_precision_ = 1 << 12;
    // Compute the total of the input frequencies.
    uint64_t total_freq = 0;
    int max_valid_symbol = 0;
    for (int i = 0; i < num_symbols; ++i) {
        total_freq += frequencies[i];
        if (frequencies[i] > 0)
            max_valid_symbol = i;
    }
    num_symbols = max_valid_symbol + 1;
    probability_table_.resize(num_symbols);
    const double total_freq_d = static_cast<double>(total_freq);
    const double rans_precision_d = static_cast<double>(rans_precision_);
    // Compute probabilities by rescaling the normalized frequencies into interval
    // [1, rans_precision - 1]. The total probability needs to be equal to
    // rans_precision.
    int total_rans_prob = 0;
    for (int i = 0; i < num_symbols; ++i) {
        const uint64_t freq = frequencies[i];

        // Normalized probability.
        const double prob = static_cast<double>(freq) / total_freq_d;

        // RAns probability in range of [1, rans_precision - 1].
        uint32_t rans_prob = static_cast<uint32_t>(prob * rans_precision_d + 0.5f);
        if (rans_prob == 0 && freq > 0)
            rans_prob = 1;
        probability_table_[i].prob = rans_prob;
        total_rans_prob += rans_prob;
    }
    // Because of rounding errors, the total precision may not be exactly accurate
    // and we may need to adjust the entries a little bit.
    if (total_rans_prob != rans_precision_) {
        std::vector<int> sorted_probabilities(num_symbols);
        for (int i = 0; i < num_symbols; ++i) {
            sorted_probabilities[i] = i;
        }
        std::sort(sorted_probabilities.begin(), sorted_probabilities.end(),
                  ProbabilityLess(&probability_table_));
        if (total_rans_prob < rans_precision_) {
            // This happens rather infrequently, just add the extra needed precision
            // to the most frequent symbol.
            probability_table_[sorted_probabilities.back()].prob +=
                    rans_precision_ - total_rans_prob;
        } else {
            // We have over-allocated the precision, which is quite common.
            // Rescale the probabilities of all symbols.
            int32_t error = total_rans_prob - rans_precision_;
            while (error > 0) {
                const double act_total_prob_d = static_cast<double>(total_rans_prob);
                const double act_rel_error_d = rans_precision_d / act_total_prob_d;
                for (int j = num_symbols - 1; j > 0; --j) {
                    int symbol_id = sorted_probabilities[j];
                    if (probability_table_[symbol_id].prob <= 1) {
                        if (j == num_symbols - 1)
                            return false;  // Most frequent symbol would be empty.
                        break;
                    }
                    const int32_t new_prob = static_cast<int32_t>(
                            floor(act_rel_error_d *
                                  static_cast<double>(probability_table_[symbol_id].prob)));
                    int32_t fix = probability_table_[symbol_id].prob - new_prob;
                    if (fix == 0u)
                        fix = 1;
                    if (fix >= static_cast<int32_t>(probability_table_[symbol_id].prob))
                        fix = probability_table_[symbol_id].prob - 1;
                    if (fix > error)
                        fix = error;
                    probability_table_[symbol_id].prob -= fix;
                    total_rans_prob -= fix;
                    error -= fix;
                    if (total_rans_prob == rans_precision_)
                        break;
                }
            }
        }
    }

    // Compute the cumulative probability (cdf).
    uint32_t total_prob = 0;
    for (int i = 0; i < num_symbols; ++i) {
        probability_table_[i].cum_prob = total_prob;
        total_prob += probability_table_[i].prob;
    }
    if (total_prob != rans_precision_)
        return false;

    // Estimate the number of bits needed to encode the input.
    // From Shannon entropy the total number of bits N is:
    //   N = -sum{i : all_symbols}(F(i) * log2(P(i)))
    // where P(i) is the normalized probability of symbol i and F(i) is the
    // symbol's frequency in the input data.
    double num_bits = 0;
    for (int i = 0; i < num_symbols; ++i) {
        if (probability_table_[i].prob == 0)
            continue;
        const double norm_prob =
                static_cast<double>(probability_table_[i].prob) / rans_precision_d;
        num_bits += static_cast<double>(frequencies[i]) * log2(norm_prob);
    }
    int num_expected_bits_ = static_cast<uint64_t>(ceil(-num_bits));
    std::cout << num_expected_bits_ / 8 << std::endl;
    return true;
}

int main(const int argc, const char **argv) {
    using namespace std;

    if (argc < 2) cout << "Please Specify Input File" << endl;
    string file_path = argv[1];

    vector<uint8_t> file_buffer;
    LoadFileBuffer(file_path, file_buffer);

    uint64_t frequencies[256];
    for (size_t i = 0; i < 256; i++) frequencies[i] = 0;
    for (uint8_t s:file_buffer) frequencies[s]++;
    int num_symbols = 0;
    for (size_t i = 0; i < 256; i++) num_symbols += (frequencies[i] != 0);
    Create(frequencies, 256);
    RAnsEncoder<12> rans_coder;
}