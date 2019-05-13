#include "Helper.h"

using namespace std;

int main(const int argc, const char **argv) {
    if (argc < 2) cout << "Please Specify Input File" << endl;
    string file_path = argv[1];

    vector<uint8_t> file_buffer;
    LoadFileBuffer(file_path, file_buffer);

    ofstream f;
    f.open(file_path + ".obj");

    size_t i = 0;
    for(; i < file_buffer.size(); i+=3) {
        f << "v " << +file_buffer[i] << " " << +file_buffer[i+1] << " " << +file_buffer[i+2] << endl;
    }
}