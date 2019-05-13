#include "Helper.h"
#include "rANSEncoder.h"

using namespace std;

int main(const int argc, const char **argv) {
    if (argc < 2) cout << "Please Specify Input File" << endl;
    string file_path = argv[1];

    vector<uint8_t> file_buffer;
    LoadFileBuffer(file_path, file_buffer);
    if (file_buffer.empty()) cout << "Read a empty file, exiting..." << endl;

    vector<uint8_t> encoded_buffer;
    rANSEncoder<12> encoder;
    encoder.Encode(file_buffer, encoded_buffer);

    SaveFileBuffer(file_path + ".rANS", encoded_buffer, false);
}