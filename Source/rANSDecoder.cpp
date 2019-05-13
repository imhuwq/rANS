#include "Symbol.h"
#include "rANSDecoder.h"

using namespace std;

void rANSDecoder::Decode(const std::vector <uint8_t> &input_buffer, std::vector <uint8_t> &decoded_buffer) {
    size_t num_symbol = input_buffer[0];
    size_t table_len = num_symbol * 9 + 1;
    vector<uint8_t> table_buffer(input_buffer.begin(), input_buffer.begin() + table_len);
    SymbolTable table = SymbolTable::FromBuffer(table_buffer);
}
