cmake_minimum_required(VERSION 3.10)
project(hw04-huffman)

set(CMAKE_CXX_STANDARD 14)

set(HuffmanLib
        src/lib/huffman_library.h
        src/lib/huffman_encoder.h
        src/lib/huffman_decoder.h
        src/lib/huffman_encoder.cpp
        src/lib/huffman_decoder.cpp
        src/lib/res/consts.h
        src/lib/res/buffer.cpp
        src/lib/res/symbol_code.cpp
        )

set(GTests
        src/gtest/gtest.h
        src/gtest/gtest-all.cc
        src/gtest/gtest_main.cc
        )

add_executable(main src/main.cpp ${HuffmanLib})
add_executable(huffman_testing src/huffman_testing.cpp ${HuffmanLib} ${GTests})

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif ()

target_link_libraries(huffman_testing -lpthread)