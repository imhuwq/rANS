#ifndef RANS_HELPER_H
#define RANS_HELPER_H

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

void LoadFileBuffer(const std::string &file_path, std::vector<uint8_t> &file_buffer);

void PrintFileBuffer(const std::vector<uint8_t> &file_buffer);

template<typename IterBeg, typename IterEnd>
void SaveFileBuffer(const std::string &file_path, IterBeg beg, IterEnd end);

void SaveFileBuffer(const std::string &file_path, const std::vector<uint8_t> &file_buffer, bool reverse);

union Uint32Buff {
    uint8_t buf[4];
    uint32_t num;
};

void SerializeUint32(uint32_t num, std::vector<uint8_t> &buffer);

uint32_t DeserializeUint32(const std::vector<uint8_t> &buffer, size_t pos);

uint32_t BitsUsed(uint32_t num);
#endif //RANS_HELPER_H
