#ifndef HUFFMAN_HUFFMAN_ENCODER_H
#define HUFFMAN_HUFFMAN_ENCODER_H

#include "res/buffer.cpp"
#include "res/symbol_code.cpp"

class huffman_encoder final {

private:
    bool begin = false;
    std::unordered_map<symbol, int> numbers;
    std::unordered_map<symbol, symbol_code> codes;
    bstring root;
    std::map<bstring, std::pair<bstring, bstring>> huffman_tree;
    std::vector<symbol> stored;
    unsigned long long remainder_size = 0;
    symbol_code last_code;

protected:
    void write_header(buffer &bout);
    void create_encode_tree();
    bool flush_data(buffer &bout);
    bool encode_data(buffer &bin, buffer &bout);

    symbol_code get_char_code(const bstring &current_symbol);

public:
    bool encode(symbol *input, size_t input_size, symbol *&output, size_t output_size, size_t &output_length);
    bool flush(symbol *&output, size_t output_size, size_t &output_length);

    void prepare_encode_tree(symbol *input, size_t input_size);
    bool last_byte(symbol &s);
};


#endif //HUFFMAN_HUFFMAN_ENCODER_H