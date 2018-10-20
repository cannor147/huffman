#include "gtest/gtest.h"
#include "lib/huffman_library.h"
#include <fstream>

namespace {
    std::string input(std::string text) {
        std::ofstream in_test("test.txt", std::ios::binary);
        in_test << text;
        in_test.close();

        // encode
        std::ifstream in_encode("test.txt", std::ios::binary);
        std::ofstream out_encode("encode.txt", std::ios::binary);
        stream_compress(in_encode, out_encode);
        in_encode.close();
        out_encode.close();

        // decode
        std::ifstream in_decode("encode.txt", std::ios::binary);
        std::ofstream out_decode("decode.txt", std::ios::binary);
        stream_decompress(in_decode, out_decode);
        in_decode.close();
        out_decode.close();

        std::ifstream out_test("decode.txt", std::ios::binary);
        std::string check((std::istreambuf_iterator<char>(out_test)),
                          std::istreambuf_iterator<char>());
        out_test.close();
        return check;
    }
}

TEST(correctness, empty) {
    std::string text = "";
    std::string check = input(text);
    EXPECT_EQ(text, check);
}
TEST(correctness, 123456) {
    std::string text = "123456";
    std::string check = input(text);
    EXPECT_EQ(text, check);
}
TEST(correctness, hello_world) {
    std::string text = "hello_world";
    std::string check = input(text);
    EXPECT_EQ(text, check);
}

namespace {
    bool generate(size_t size) {
        std::ofstream in_test("test.txt", std::ios::binary);
        char* in = new char[size];
        for (size_t i = 0; i < size; ++i) {
            in[i] = (char) rand();
        }
        in_test.write(in, size);
        in_test.close();

        // encode
        std::ifstream in_encode("test.txt", std::ios::binary);
        std::ofstream out_encode("encode.txt", std::ios::binary);
        stream_compress(in_encode, out_encode);
        in_encode.close();
        out_encode.close();

        // decode
        std::ifstream in_decode("encode.txt", std::ios::binary);
        std::ofstream out_decode("decode.txt", std::ios::binary);
        stream_decompress(in_decode, out_decode);
        in_decode.close();
        out_decode.close();

        std::ifstream out_test("decode.txt", std::ios::binary);
        char* out = new char[size];
        out_test.read(out, size);
        bool result = true;
        for (size_t i = 0; i < size; ++i) {
            if (in[i] != out[i]) {
                result = false;
                break;
            }
        }
        delete[] in;
        delete[] out;
        return result;
    }
}

TEST(correctness, block_size) {
    EXPECT_TRUE(generate(block_size));
}
TEST(correctness, 256) {
    for (size_t i = 32; i < 256; i++) {
        EXPECT_TRUE(generate(256));
    }
}
TEST(correctness, very_big_size) {
    EXPECT_TRUE(generate(80 * 1024 * 1024));
}