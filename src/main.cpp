#include "lib/huffman_library.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <ctime>

std::string count_time(double f) {
    unsigned long long h = 0, m = 0, s = static_cast<unsigned long long>(f);
    if (s > 60) {
        m = s / 60;
        s %= 60;
        if (m > 60) {
            h = m / 60;
            m %= 60;
        }
    }
    std::string result = "";
    if (h > 0) result += std::to_string(h) + "h ";
    if (m > 0) result += std::to_string(m) + "m ";
    result += std::to_string(s) + "s ";
    return result;
}

void compress(std::ifstream &in, std::ofstream &out) {
    std::cout << "compressing.." << std::endl;
    time_t start;
    time(&start);
    stream_compress(in, out);
    time_t stop;
    time(&stop);
    double f = difftime(stop, start);
    std::cout << "Compression time: " << count_time(f) << std::endl;
    std::cout << std::endl;
}
void decompress(std::ifstream &in, std::ofstream &out) {
    std::cout << "decompressing.." << std::endl;
    time_t start;
    time(&start);
    stream_decompress(in, out);
    time_t stop;
    time(&stop);
    double f = difftime(stop, start);
    std::cout << "Decompression time: " << count_time(f) << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (argc == 4) {
            std::string filein = argv[2], fileout = argv[3];
            std::ofstream out(fileout, std::ios::binary);
            std::ifstream in(filein, std::ios::binary);
            if (!in.is_open() || !out.is_open()) {
                std::cout << "Error #130: file is not able to opening" << std::endl;
                return -1;
            }

            if (!strcmp(argv[1], "--compress")) {
                compress(in, out);
            } else if (!strcmp(argv[1], "--decompress")) {
                decompress(in, out);
            } else {
                std::cout << "Error #130: file is not able to opening" << std::endl;
                return -1;
            }
        }
    }
    return 0;
}

