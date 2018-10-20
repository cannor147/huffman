#ifndef HUFFMAN_SYMBOLCODE_CPP
#define HUFFMAN_SYMBOLCODE_CPP

#include "consts.h"
#include "buffer.cpp"

struct symbol_code {

    size_t length;
    std::vector<bitset> bits;

    symbol_code() {
        length = 0;
    }
    explicit symbol_code(size_t l) {
        length = l;
    }
    bool get_bit(size_t index) {
        return bits[index / bitset_size][index % bitset_size];
    }
    void clear() {
        length = 0;
        bits.clear();
    }
    void write(buffer &bout, size_t l = 0) {
        if (l == 0) l = length;
        for (auto &bit : bits) {
            bout.write_bitset(bit, std::min(l, bitset_size));
            l -= std::min(l, bitset_size);
            if (l == 0) break;
        }
    }
    symbol_code update_last(size_t l) {
        symbol_code result(l);
        size_t start = length - l;
        size_t bits_index = start / bitset_size;
        size_t bit_index = start % bitset_size;
        size_t shifted = bitset_size - bit_index;
        while (l > 0) {
            bitset my_bits = bits[bits_index];
            my_bits >>= bit_index;
            l -= std::min(shifted, result.length);
            if (l > 0) {
                if (bits_index > bits.size() - 1) {
                    throw std::runtime_error("Error #473: very big length");
                }
                size_t num = std::min(bit_index, l);
                for (size_t i = 0; i < num; i++) {
                    my_bits[shifted + i] = bits[bits_index][i];
                }
                l -= num;
            }
            result.bits.push_back(my_bits);
            bits_index++;
        }
        return result;
    }
};

#endif //HUFFMAN_SYMBOLCODE_CPP