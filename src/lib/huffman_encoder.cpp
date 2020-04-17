#include "huffman_encoder.h"

void huffman_encoder::write_header(buffer &bout) {
    bout.write_number(codes.size());
    for (auto &code : codes) {
        bout.write_symbol(code.first);
        bout.write_symbol(static_cast<symbol>(codes[code.first].length));
    }
    for (auto &code : codes) {
        codes[code.first].write(bout);
    }
    bout.write_long_number(remainder_size);
    begin = true;
}
void huffman_encoder::create_encode_tree() {
    if(numbers.empty()) {
        std::cout << "Error #201: empty input file" << std::endl;
        exit(0);
    }
    std::priority_queue<std::pair<int, bstring>> tree;
    for (auto &number : numbers) {
        bstring current_symbol;
        current_symbol.push_back(number.first);
        tree.push(make_pair(-number.second, current_symbol));
    }
    while (tree.size() >= 2) {
        auto pair1 = tree.top();
        tree.pop();
        auto pair2 = tree.top();
        tree.pop();
        auto new_vertex = make_pair(pair1.first + pair2.first, pair1.second + pair2.second);
        huffman_tree[new_vertex.second] = make_pair(pair1.second, pair2.second);
        tree.push(new_vertex);
    }
    root = tree.top().second;
    for (auto &number : numbers) {
        bstring current_symbol;
        current_symbol += (number.first);
        codes[number.first] = get_char_code(current_symbol);
    }
}
bool huffman_encoder::encode_data(buffer &bin, buffer &bout) {
    bin.reset();
    bool tail = false;
    long long bit_num = bout.get_other_capacity();
    bin.reset();
    if (last_code.length > 0) {
        last_code.write(bout);
        bit_num -= last_code.length;
        last_code.clear();
    }
    while (bin.has_symbol() && bit_num > 0) {
        symbol s = bin.read_symbol();
        symbol_code code = codes[s];
        if (bit_num > code.length) {
            code.write(bout);
            bit_num -= code.length;
        } else {
            last_code = code.update_last(code.length - (size_t) bit_num);
            code.write(bout, static_cast<size_t>(bit_num));
            break;
        }
    }
    if (bit_num > 0 && bout.get_bitindex()) {
        last_code.length = bout.get_bitindex();
        last_code.bits.push_back(bout.pop_tail());
    }
    while (bin.has_symbol()) {
        symbol s = bin.read_symbol();
        stored.push_back(s);
        tail = true;
    }
    return tail;
}
bool huffman_encoder::flush_data(buffer &bout) {
    unsigned long long bit_num = bout.get_other_capacity();
    if (bit_num < last_code.length) {
        throw std::runtime_error("Error #211: wrong output buffer");
    }
    if (last_code.length > 0) {
        last_code.write(bout);
        bit_num -= last_code.length;
        last_code.clear();
    }
    auto it = stored.begin();
    while (it != stored.end() && bit_num > 0) {
        symbol s = *it;
        symbol_code code = codes[s];
        if (bit_num > code.length) {
            code.write(bout);
            bit_num -= code.length;
        } else {
            last_code = code.update_last(code.length - (size_t) bit_num);
            code.write(bout, static_cast<size_t>(bit_num));
            break;
        }
        it++;
    }
    if (bit_num > 0 && bout.get_bitindex()) {
        last_code.length = bout.get_bitindex();
        last_code.bits.push_back(bout.pop_tail());
    }
    std::vector<symbol> remchars;
    bool t = false;
    while (it != stored.end()) {
        remchars.push_back(*it);
        t = true;
        it++;
    }
    stored.swap(remchars);
    return t;
}

bool huffman_encoder::encode(symbol *input, size_t input_size, symbol *&output, size_t output_size, size_t &output_length) {
    buffer bin(input, input_size);
    if (output == nullptr) {
        output_size = std::max(input_size, static_cast<size_t>(1024));
        output = new symbol[output_size];
    }
    buffer bout(output, output_size);
    if (!begin) {
        create_encode_tree();
        write_header(bout);
    }
    bool result = encode_data(bin, bout);
    output_length = bout.get_position();
    return result;
}
bool huffman_encoder::flush(symbol *&output, size_t output_size, size_t &output_length) {
    if (output == nullptr) {
        output_size = stored.size() + (last_code.length + bitset_size - 1) / bitset_size;
        output = new symbol[output_size];
    }
    buffer bout(output, output_size);
    bool result = flush_data(bout);
    output_length = bout.get_position();
    return result;
}

void huffman_encoder::prepare_encode_tree(symbol *input, size_t input_size) {
    buffer in(input, input_size);
    while (in.has_symbol()) {
        numbers[in.read_symbol()]++;
        remainder_size++;
    }
}
bool huffman_encoder::last_byte(symbol &s) {
    if (last_code.length == 0) return false;
    if (last_code.length <= bitset_size) {
        s = last_code.bits[0].to_ulong() & 255;
    } else {
        throw std::runtime_error("Error #209: wrong end of file");
    }
    return true;
}
symbol_code huffman_encoder::get_char_code(const bstring &current_symbol) {
    bstring current_vertex = root;
    std::vector<bool> my_bits;
    while (current_vertex.length() > 1) {
        bstring left = huffman_tree[current_vertex].first, right = huffman_tree[current_vertex].second;
        if (left.find(current_symbol) != bstring::npos) {
            current_vertex = left;
            my_bits.push_back(false);
        } else if (right.find(current_symbol) != bstring::npos) {
            current_vertex = right;
            my_bits.push_back(true);
        } else {
            throw std::runtime_error("Error #205: ambiguity");
        }
    }
    symbol_code result;
    result.length = my_bits.size();
    size_t j = 0;
    while (j < result.length) {
        bitset curbits;
        size_t current_length = std::min(result.length - j, bitset_size);
        for (size_t i = 0; i < current_length; i++) {
            curbits[i] = my_bits[i + j];
        }
        result.bits.push_back(curbits);
        j += current_length;
    }
    return result;
}