cmake_minimum_required(VERSION 3.10)
project(huffman)

set(CMAKE_CXX_STANDARD 14)

set(HuffmanLib
        lib/huffman_library.h
        lib/huffman_encoder.h
        lib/huffman_decoder.h
        lib/huffman_encoder.cpp
        lib/huffman_decoder.cpp
        lib/res/buffer.cpp
        lib/res/symbol_code.cpp
        )

set(GTests
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc
        )

add_executable(main main.cpp ${HuffmanLib})
add_executable(test huffman_testing.cpp ${HuffmanLib} ${GTests})