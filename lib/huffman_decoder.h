#ifndef HUFFMAN_HUFFMAN_DECODER_H
#define HUFFMAN_HUFFMAN_DECODER_H

#include "res/buffer.cpp"
#include "res/symbol_code.cpp"

struct vertex {
    bool final = false;
    unsigned char value;
    std::unique_ptr<vertex> left, right;
};

class huffman_decoder final {

public:
    bool begin = false;
    std::map<symbol, symbol_code> codes;
    vertex root;
    std::vector<symbol> stored;
    unsigned long long remainder_size;
    std::bitset<bitset_size> last_bitset;
    size_t last_bit = bitset_size;

protected:
    void read_header(buffer &bin);
    void create_decode_tree();
    bool decode_data(buffer &bin, buffer &bout);
    bool flush_data(buffer &bout);

public:
    bool decode(symbol *input, size_t input_size, symbol *&output, size_t output_size, size_t &output_length);
    bool flush(symbol *&output, size_t &output_size, size_t &output_length);
};

#endif //HUFFMAN_HUFFMAN_DECODER_H