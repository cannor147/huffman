#ifndef HUFFMAN_BUFFER_CPP
#define HUFFMAN_BUFFER_CPP

#include "consts.h"

class buffer final {
    symbol *my_symbol;
    size_t capacity;
    size_t char_index;
    size_t bit_index;

public:
    buffer() {
        my_symbol = nullptr;
        capacity = char_index = bit_index = 0;
    }
    buffer(symbol *s, size_t c) {
        my_symbol = s;
        capacity = c;
        char_index = bit_index = 0;
    }
    size_t get_bitindex() {
        return bit_index;
    }
    size_t get_position() {
        return char_index + (bit_index > 0);
    }
    size_t get_capacity() {
        return capacity;
    }
    unsigned long long get_other_capacity() {
        return (capacity - char_index) * (unsigned long long)bitset_size - bit_index;
    }
    void reset() {
        char_index = bit_index = 0;
    }

    // readers:
    symbol read_symbol() {
        if (bit_index) {
            char_index++;
            bit_index = 0;
        }
        symbol *result = my_symbol + char_index;
        char_index++;
        return *result;
    }
    size_t read_number() {
        size_t result = 0;
        size_t shift = 0;
        for (size_t i = 0; i < sizeof(size_t); i++) {
            auto cur = static_cast<size_t>(read_symbol());
            result += cur << shift;
            shift += bitset_size;
        }
        return result;
    }
    unsigned long long read_long_number() {
        unsigned long long result = read_number();
        result <<= 32;
        result += read_number();
        return result;
    }
    bitset read_bitset(size_t length) {
        if (length > bitset_size) {
            throw std::runtime_error("Too big bitset!");
        }
        symbol *symb = my_symbol + char_index;
        bitset bit_symb(*symb);
        bitset result;
        size_t curlen = bitset_size - bit_index;
        int remlen = static_cast<int>(length - curlen);
        curlen = std::min(curlen, length);
        for (size_t i = 0; i < curlen; i++) {
            result.set(i, bit_symb[i + bit_index]);
        }
        bit_index = (bit_index + curlen) % bitset_size;
        if (bit_index == 0) char_index++;
        if (remlen <= 0) return result;
        symb = my_symbol + char_index;
        bitset rembits(*symb);
        for (size_t i = 0; i < remlen; i++) {
            result.set(i + curlen, rembits[i]);
            bit_index++;
        }
        return result;
    }
    std::vector<bitset> read_bitsets(size_t length) {
        std::vector<bitset> result;
        size_t result_size = 0;
        while (result_size < length) {
            result.push_back(read_bitset(std::min(length - result_size, bitset_size)));
            result_size += result.back().size();
        }
        return result;
    }

    // writers:
    void write_symbol(symbol c) {
        if (bit_index) {
            char_index++;
            bit_index = 0;
        }
        symbol *curp = my_symbol + char_index;
        *curp = c;
        char_index++;
    }
    void write_number(size_t value) {
        for (size_t i = 0; i < sizeof(size_t); i++) {
            auto curc = static_cast<symbol>(value & 255);
            write_symbol(curc);
            value >>= bitset_size;
        }
    }
    void write_long_number(unsigned long long value) {
        write_number(value >> 32);
        write_number(value & 0xFFFFFFFF);
    }
    void write_bitset(const bitset &bits, size_t length) {
        if (length > bitset_size) {
            throw std::runtime_error("Too big bitset!");
        }
        symbol *symb = my_symbol + char_index;
        bitset bit_symb(*symb);
        size_t curlen = bitset_size - bit_index;
        int remlen = static_cast<int>(length - curlen);
        curlen = std::min(curlen, length);
        for (size_t i = 0; i < curlen; i++) {
            bit_symb.set(i + bit_index, bits[i]);
        }
        *symb = bit_symb.to_ulong() & 255;
        bit_index = (bit_index + curlen) % bitset_size;
        if (bit_index == 0) char_index++;
        if (remlen > 0) {
            symb = my_symbol + char_index;
            bit_symb.reset();
            for (size_t i = 0; i < remlen; i++) {
                bit_symb.set(i + bit_index, bits[i + curlen]);
            }
            bit_index += remlen;
            *symb = bit_symb.to_ulong() & 255;
        }

    }

    bool has_symbol() {
        return get_position() < capacity;
    }
    bitset pop_tail() {
        symbol *symb = my_symbol + char_index;
        bitset bit_symb(*symb), result;
        for (size_t i = 0; i < bit_index; i++) {
            result.set(i, bit_symb[i]);
        }
        bit_index = 0;
        return result;
    }
};

#endif //HUFFMAN_BUFFER_CPP