cmake_minimum_required(VERSION 3.10)
project(huffman)

set(CMAKE_CXX_STANDARD 14)

set(HuffmanLib
        lib/huffman_library.h
        lib/huffman_encoder.h
        lib/huffman_decoder.h
        lib/huffman_encoder.cpp
        lib/huffman_decoder.cpp
        lib/res/consts.h
        lib/res/buffer.cpp
        lib/res/symbol_code.cpp
        )

set(GTests
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc
        )

add_executable(main main.cpp ${HuffmanLib})
add_executable(huffman_testing huffman_testing.cpp ${HuffmanLib} ${GTests})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(huffman_testing -lpthread)