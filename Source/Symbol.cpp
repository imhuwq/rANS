#include "Symbol.h"

using namespace std;

const Symbol &SymbolTable::operator[](uint8_t symbol) {
    return symbols_.at(symbol);
}

std::vector<uint8_t> SymbolTable::ToBuffer(const SymbolTable &table) {
    std::vector<uint8_t> buffer;

    SerializeUint32(table.symbols_.size(), buffer);

    for (const auto &pair: table.symbols_) {
        uint8_t symbol = pair.first;
        Symbol detail = pair.second;

        buffer.push_back(symbol);
        SerializeUint32(detail.freq_, buffer);
        SerializeUint32(detail.begin_, buffer);
    }

    return buffer;
}

SymbolTable SymbolTable::FromBuffer(std::vector<uint8_t> buffer) {
    SymbolTable table;
    size_t table_size = 0;

    size_t pos = 0;
    size_t num_symbols = DeserializeUint32(buffer, pos++);

    for (size_t i = 0; i < num_symbols; i++) {
        uint8_t s = buffer[pos++];
        size_t freq = DeserializeUint32(buffer, pos);
        pos += 4;
        size_t begin = DeserializeUint32(buffer, pos);
        pos += 4;

        Symbol symbol(s, freq, begin);
        table.symbols_.insert(std::make_pair(s, symbol));
        table_size += freq;
    }

    return table;
}

void SymbolTable::BuildTable(const std::vector<uint8_t> &symbol_buffer) {
    vector<Symbol> table(256);
    for (uint8_t s:symbol_buffer) {
        table[s].s_ = s;
        table[s].freq_++;
    }

    size_t total_prob = 0;
    size_t unique_symbol_count = 0;
    auto symbol_count = (double) symbol_buffer.size();
    for (auto &s: table) {
        if (s.freq_ == 0) continue;

        auto prob = (size_t) (s.freq_ / symbol_count * ans_precision_ + 0.5);
        if (prob == 0 && s.freq_ > 0) prob = 1;
        s.prob_ = prob;  // normalized
        total_prob += prob;
        unique_symbol_count++;
    }

    if (total_prob != ans_precision_) {
        // adjust prob table
        vector<uint8_t> symbols_sorted_by_freq(256);
        for (size_t i = 0; i < symbols_sorted_by_freq.size(); i++) symbols_sorted_by_freq[i] = i;
        auto comp = [table](uint8_t i, uint8_t j) -> bool { return table[i].prob_ > table[j].prob_; };
        std::sort(symbols_sorted_by_freq.begin(),
                  symbols_sorted_by_freq.end(),
                  comp);

        if (total_prob < ans_precision_) {
            Symbol &s = symbols_[symbols_sorted_by_freq.front()];
            s.prob_ += (ans_precision_ - total_prob);
        } else {
            size_t error = total_prob - ans_precision_;
            double total_prob_d = total_prob;
            double fix_ratio = ans_precision_ / total_prob_d;

            for (size_t i = 0; i < unique_symbol_count; i++) {
                uint8_t s = symbols_sorted_by_freq[i];
                Symbol &symbol = table[s];
                if (symbol.prob_ <= 1) {
                    if (i == 0) exit(1); // most freq symbol is of 0 freq
                    break; // stop fix symbols of 0 freq
                };

                size_t new_freq = floor(fix_ratio * symbol.prob_);
                size_t fix_value = symbol.prob_ - new_freq;

                // adjust fix
                if (fix_value == 0) fix_value = 1;
                if (fix_value > symbol.prob_) fix_value = symbol.prob_ - 1;
                if (fix_value > error) fix_value = error;

                // apply fix
                symbol.prob_ -= fix_value;
                error -= fix_value;
                total_prob -= fix_value;
                if (total_prob == ans_precision_) break;
            }
        }
    }

    total_prob = 0;
    for (auto &symbol:table) {
        if (symbol.prob_ == 0) continue;
        Symbol new_symbol(symbol.s_, symbol.freq_, total_prob, symbol.prob_);
        symbols_.insert(make_pair(symbol.s_, new_symbol));
        total_prob += symbol.prob_;
    }

    double __debug_total_bits = 0;
    for (const auto p: symbols_) {
        Symbol s = p.second;
        double prob = double(s.prob_) / double(total_prob);
        __debug_total_bits += (-log2(prob) * s.freq_);
    }

    std::cout << "Minimum Bytes Needed: " << __debug_total_bits / 8 << " Bytes" << std::endl;
}

