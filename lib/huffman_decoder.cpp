#include "huffman_decoder.h"

void huffman_decoder::read_header(buffer &bin) {
    size_t table_size = bin.read_number();
    std::vector<std::pair<symbol, size_t>> table;
    for (size_t i = 0; i < table_size; i++) {
        if (!bin.has_symbol()) {
            std::cout << "Error #201: incorrect input file" << std::endl;
            exit(0);
        }
        symbol c = bin.read_symbol();
        if (!bin.has_symbol()) {
            std::cout << "Bad input!" << std::endl;
        }
        table.emplace_back(c, static_cast<size_t>(bin.read_symbol()));
    }
    for (auto item : table) {
        codes[item.first].length = item.second;
        codes[item.first].bits = bin.read_bitsets(item.second);
    }
    remainder_size = bin.read_long_number();
    begin = true;
}
void huffman_decoder::create_decode_tree() {
    for (auto &code : codes) {
        vertex *current_vertex = &root, *next_vertex;
        for (size_t i = 0; i < code.second.length; i++) {
            if (code.second.get_bit(i)) {
                next_vertex = current_vertex->right.get();
                if (next_vertex == nullptr) {
                    next_vertex = new vertex;
                    current_vertex->right.reset(next_vertex);
                }
            } else {
                next_vertex = current_vertex->left.get();
                if (next_vertex == nullptr) {
                    next_vertex = new vertex;
                    current_vertex->left.reset(next_vertex);
                }
            }
            if (next_vertex->final) {
                throw std::runtime_error("Ambiguity!");
            }
            current_vertex = next_vertex;
        }
        current_vertex->final = true;
        current_vertex->value = code.first;
    }
}
bool huffman_decoder::decode_data(buffer &bin, buffer &bout) {
    size_t capacity = bout.get_capacity();
    vertex *current_vertex = &root;
    bitset current_bits;
    for (size_t i = last_bit; i < bitset_size; i++) {
        current_vertex = (last_bitset[i] ? current_vertex->right.get() : current_vertex->left.get());
        if (!current_vertex) {
            std::cout << "Bad code!" << std::endl;
            exit(0);
        }
        if (current_vertex->final) {
            throw std::runtime_error("Decoding: incorret last bits from previous buffer!");
        }
    }
    size_t last = 0;
    while (remainder_size != 0 && bin.has_symbol()) {
        current_bits = bin.read_bitset(bitset_size);
        for (size_t i = 0; i < bitset_size; i++) {
            if (current_bits[i]) {
                current_vertex = current_vertex->right.get();
            } else {
                current_vertex = current_vertex->left.get();
            }
            if (current_vertex->final) {
                last = i;
                if (capacity > 0) {
                    bout.write_symbol(current_vertex->value);
                } else {
                    stored.push_back(current_vertex->value);
                }
                current_vertex = &root;
                if(capacity > 0) capacity--;
                if(remainder_size > 0) {
                    remainder_size--;
                } else {
                    break;
                }
            }
        }
    }
    if (remainder_size > 0) {
        last_bitset = current_bits;
        last_bit = last + 1;
    }
    return !stored.empty();
}
bool huffman_decoder::flush_data(buffer &bout) {
    size_t capacity = bout.get_capacity();
    auto it = stored.begin();
    std::vector<symbol> rem_chars;
    while (it != stored.end()) {
        if (capacity > 0) {
            bout.write_symbol(*it);
        } else {
            rem_chars.push_back(*it);
        }
        if (capacity > 0) capacity--;
        it++;
    }
    stored.swap(rem_chars);
    return !stored.empty();
}

bool huffman_decoder::decode(symbol *input, size_t input_size, symbol *&output, size_t output_size, size_t &output_length) {
    buffer bin(input, input_size);
    if (output == nullptr) {
        output_size = static_cast<size_t>(remainder_size);
        output = new symbol[output_size];
    }
    buffer bout(output, output_size);
    if (!begin) {
        read_header(bin);
        create_decode_tree();
    }
    bool result = decode_data(bin, bout);
    output_length = bout.get_position();
    return result;
}
bool huffman_decoder::flush(symbol *&output, size_t &output_size, size_t &output_length) {
    if (output == nullptr) {
        output_size = stored.size();
        output = new symbol[output_size];
    }
    buffer bout(output, output_size);
    bool result = flush_data(bout);
    output_length = bout.get_position();
    return result;
}