#ifndef RANS_SYMBOL_H
#define RANS_SYMBOL_H

#include <map>
#include <vector>

#include "Helper.h"

class Symbol {
public:
    Symbol(uint8_t s, size_t freq, size_t begin) :
            s_(s), freq_(freq), begin_(begin), prob_(0) {}

    Symbol() : s_(0), freq_(0), begin_(0), prob_(0) {}

    Symbol(uint8_t s, size_t freq, size_t begin, size_t prob) :
            s_(s), freq_(freq), begin_(begin), prob_(prob) {}

    // TODO: use smaller width for freq_ and prob_ and begin_ to decrease table size
    uint8_t s_;  // symbol
    size_t freq_; // frequency of symbol
    size_t prob_; // normalized probability of symbol
    size_t begin_; // begin position of symbol in table
};

class SymbolTable {
public:
    explicit SymbolTable(const std::vector<uint8_t> &symbol_buffer,
                         int ans_precision_) :
            ans_precision_(ans_precision_) {
        BuildTable(symbol_buffer);
    }

    SymbolTable() {}

    size_t num_symbols() { return symbols_.size(); }

    const Symbol &operator[](uint8_t symbol);

    static std::vector<uint8_t> ToBuffer(const SymbolTable &table);

    static SymbolTable FromBuffer(std::vector<uint8_t> buffer);

private:
    int ans_precision_;

    std::map<uint8_t, Symbol> symbols_;

    void BuildTable(const std::vector<uint8_t> &symbol_buffer);
};

#endif //RANS_SYMBOL_H
