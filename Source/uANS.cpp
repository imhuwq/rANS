#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

class Buffer {
public:
    char *buf_data;
    size_t buf_len;

    explicit Buffer(size_t len) : buf_len(len) {
        buf_data = (char *) malloc(len);
    }

    Buffer(char *data, size_t len) : buf_len(len) {
        buf_data = (char *) malloc(len);
        memcpy(buf_data, data, len);
    }

    Buffer(const Buffer &rhs) : Buffer(rhs.buf_data, rhs.buf_len) {}

    Buffer &operator=(const Buffer &rhs) {
        buf_len = rhs.buf_len;
        memcpy(buf_data, rhs.buf_data, rhs.buf_len);
        return *this;
    }

    ~Buffer() { delete buf_data; }
};

class NumStream {
private:
    Buffer *buffer;

public:
    explicit NumStream(size_t len) : buffer(new Buffer(len)) {}

    ~NumStream() { delete buffer; };
};

struct ProbTable {
    double p0;
    double p1;
    double val;

    explicit ProbTable(Buffer const *buffer) {
        size_t counter_0 = 0;
        for (size_t i = 0; i < buffer->buf_len; i++) {
            counter_0 += (buffer->buf_data[i] == '0');
        }
        p0 = (buffer->buf_len - counter_0) / double(buffer->buf_len);
        p1 = 1.0 - p0;

        double info_val_0 = (double) counter_0 / p0;
        double info_val_1 = (double) (buffer->buf_len - counter_0) / p1;
        val = ceil(info_val_0 + info_val_1);
    }

    void Print() {
        printf("%-20s:: %f\n", "Probability 0", p0);
        printf("%-20s:: %f\n", "Probability 1", p1);
        printf("%-20s:: %f\n", "Info Val", val);
    }
};

size_t Encode(Buffer const *msg_buffer, ProbTable const *table, NumStream *enc_buffer) {
    size_t enc_buff_bytes = pow(2, ceil(log2(table->val))) / 8;
    enc_buffer = new NumStream(enc_buff_bytes);

    size_t encoded = 1;
    for (size_t i = 0; i < msg_buffer->buf_len; i++) {
        if (msg_buffer->buf_data[i] == '0') {
            encoded = (size_t) ceil((encoded + 1) / table->p0) - 1;
        } else {
            encoded = (size_t) floor(encoded / table->p1);
        }
    }

    return encoded;
}

int main() {
#define LEN 20
    char msg[LEN + 1] = "01111011011100110100";
    Buffer msg_buffer(msg, LEN);
    ProbTable table(&msg_buffer);
    table.Print();

    NumStream *enc_buffer;
//    Encode(&msg_buffer, table, enc_buffer);
//
//    char* decoded_buffer;
//    Decode(symbol_table, encoded_buffer, LEN, decoded_buffer);
//    printf("%s\n", decoded);
//    printf("%s\n", symbols);
}