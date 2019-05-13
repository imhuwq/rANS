#include "Helper.h"
#include "rANSDecoder.h"

using namespace std;

int main(const int argc, const char **argv) {
    if (argc < 2) cout << "Please Specify Input File" << endl;
    string file_path = argv[1];

    vector<uint8_t> file_buffer;
    LoadFileBuffer(file_path, file_buffer);
    if (file_buffer.empty()) cout << "Read a empty file, exiting..." << endl;

    vector<uint8_t> decoded_buffer;
    rANSDecoder decoder;
    decoder.Decode(file_buffer, decoded_buffer);

    SaveFileBuffer(file_path + ".txt", decoded_buffer, true);
}