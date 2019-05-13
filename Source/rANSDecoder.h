#ifndef RANS_RANSDECODER_H
#define RANS_RANSDECODER_H

#include "Helper.h"

class rANSDecoder {
public:
    rANSDecoder() : state_(0) {}

    void Decode(const std::vector <uint8_t> &input_buffer, std::vector <uint8_t> &decoded_buffer);
private:
    uint32_t state_;
};

#endif //RANS_RANSDECODER_H
