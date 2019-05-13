#ifndef RANS_RANSENCODER_H
#define RANS_RANSENCODER_H

#include "Helper.h"
#include "Symbol.h"

template<uint32_t precision_bit>
class rANSEncoder {
public:
    rANSEncoder() { state_ = l_rans_base_; }

    void Encode(const std::vector<uint8_t> &input_buffer, std::vector<uint8_t> &encoded_buffer) {
        SymbolTable symbol_table(input_buffer, rans_precision_);
        encoded_buffer = std::move(SymbolTable::ToBuffer(symbol_table));
        size_t __debug_table_bytes = encoded_buffer.size();

        state_ = l_rans_base_;
        for (uint8_t s: input_buffer) {
            const Symbol &symbol = symbol_table[s];

            while (state_ >= l_rans_base_ / rans_precision_ * IOBase * symbol.prob_) {
                encoded_buffer.push_back(state_ % 256);
                state_ /= 256;
            }
            state_ = rans_precision_ * (state_ / symbol.prob_) + state_ % symbol.prob_ + symbol.begin_;
        }

        while (state_ > IOBase) {
            encoded_buffer.push_back(state_ % IOBase);
            state_ = state_ / IOBase;
        }
        encoded_buffer.push_back(state_);

        size_t __debug_total_bytes = encoded_buffer.size();
        std::cout << "Actual Bytes Used: " << __debug_total_bytes - __debug_table_bytes << " Bytes" << std::endl;
        std::cout << "Size of Table: " << __debug_table_bytes << " Bytes" << std::endl;
        std::cout << "Size of All: " << __debug_total_bytes << " Bytes" << std::endl;
    }

    const static size_t IOBase = 1 << 8;

private:
    uint32_t state_;
    uint32_t rans_precision_bit = precision_bit;
    uint32_t rans_precision_ = 1 << rans_precision_bit;
    uint32_t l_rans_base_ = rans_precision_ * 4;
};

#endif //RANS_RANSENCODER_H
