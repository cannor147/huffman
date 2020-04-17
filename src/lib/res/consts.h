#ifndef HUFFMAN_CONSTS_H
#define HUFFMAN_CONSTS_H

#include <iostream>
#include <vector>
#include <bitset>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>

using symbol = unsigned char;
using bstring = std::basic_string<symbol>;

static const size_t bitset_size = sizeof(symbol) * 8;
using bitset = std::bitset<bitset_size>;

static const size_t block_size = 4 * 1024 * 1024;

#endif //HUFFMAN_CONSTS_H