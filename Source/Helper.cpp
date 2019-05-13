#include "Helper.h"

void LoadFileBuffer(const std::string &file_path, std::vector<uint8_t> &file_buffer) {
    std::cout << "Loading file: " << file_path << std::endl;

    std::ifstream input_file;
    input_file.open(file_path);
    if (!input_file.eof() && !input_file.fail()) {
        input_file.seekg(0, std::ios::end);
        size_t file_size = input_file.tellg();
        input_file.seekg(0, std::ios::beg);

        file_buffer.reserve(file_size);
        file_buffer.insert(file_buffer.begin(),
                           std::istreambuf_iterator<char>(input_file),
                           std::istreambuf_iterator<char>());
    } else {
        std::cerr << "Failed to open file for reading." << std::endl;
    }

    input_file.close();
}

void PrintFileBuffer(const std::vector<uint8_t> &file_buffer) {
    for (const char &c: file_buffer) std::cout << c;
    std::cout << std::endl;
    std::cout << "\n==========\nTotal Size: " << file_buffer.size() << std::endl;
}

template<typename IterBeg, typename IterEnd>
void SaveFileBuffer(const std::string &file_path, IterBeg beg, IterEnd end) {
    std::ofstream output_file;
    output_file.open(file_path);

    if (!output_file.fail()) {
        while (beg != end) {
            output_file << *beg;
            beg++;
        }

    } else {
        std::cerr << "Fail to open file for writing..." << std::endl;
    }
}

void SaveFileBuffer(const std::string &file_path, const std::vector<uint8_t> &file_buffer, bool reverse) {
    if (reverse) {
        auto beg = file_buffer.rbegin();
        auto end = file_buffer.rend();
        SaveFileBuffer(file_path, beg, end);
    } else {
        auto beg = file_buffer.begin();
        auto end = file_buffer.end();
        SaveFileBuffer(file_path, beg, end);
    }
}

void SerializeUint32(uint32_t num, std::vector<uint8_t> &buffer) {
    static Uint32Buff transform;
    transform.num = num;
    for (uint8_t buf : transform.buf) {
        buffer.push_back(buf);
    }
};

uint32_t DeserializeUint32(const std::vector<uint8_t> &buffer, size_t pos) {
    static Uint32Buff transform;
    for (size_t i = 0; i < sizeof(transform.buf); i++) {
        transform.buf[i] = buffer[pos++];
    }
    return transform.num;
}

uint32_t BitsUsed(uint32_t num) {
    return 32 - __builtin_clz(num);
}