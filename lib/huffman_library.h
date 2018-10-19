#ifndef HUFFMAN_HUFFMAN_LIBRARY_H
#define HUFFMAN_HUFFMAN_LIBRARY_H

#include "huffman_decoder.h"
#include "huffman_encoder.h"

size_t ReadBuf(std::istream& in, char * curblock, size_t block_size) {
    in.read(curblock, block_size);
    return (size_t)in.gcount();
}

void stream_compress(std::istream &in, std::ostream &out) {
    std::unique_ptr<huffman_encoder> my_encoder(new huffman_encoder);

    size_t input_length = block_size, output_size = block_size;
    auto deleter = [&](symbol *my_ptr){
        delete [] my_ptr;
    };
    std::unique_ptr<symbol, decltype(deleter)> input_ptr(new symbol[input_length], deleter );
    std::unique_ptr<symbol, decltype(deleter)> output_ptr(new symbol[output_size], deleter);
    symbol *input = input_ptr.get(), *output = output_ptr.get();

    size_t input_size = 0;
    while ((input_size = (size_t)ReadBuf(in, (char*)input_ptr.get(), input_length)) > 0) {
        my_encoder->prepare_encode_tree(input_ptr.get(), input_size);
    }
    in.clear();
    in.seekg(0, in.beg);
    while ((input_size = (size_t)ReadBuf(in, (char*)input, input_length)) > 0) {
        size_t output_length = 0;
        bool t = my_encoder->encode(input, input_size, output, output_size, output_length);
        out.write((char*)output, output_length);
        while (t) {
            t = my_encoder->flush(output, output_size, output_length);
            out.write((char*)output, output_length);
        }
    }

    symbol s;
    if (my_encoder->last_byte(s)) out.put(s);
}
void stream_decompress(std::istream &in, std::ostream &out) {
    std::unique_ptr<huffman_decoder> my_decoder(new huffman_decoder);

    size_t input_length = block_size, output_size = block_size;
    auto deleter = [&](symbol *my_ptr){
        delete [] my_ptr;
    };
    std::unique_ptr<symbol, decltype(deleter)> input_ptr(new symbol[input_length], deleter);
    std::unique_ptr<symbol, decltype(deleter)> output_ptr(new symbol[output_size], deleter);
    symbol *input = input_ptr.get(), *output = output_ptr.get();

    size_t input_size = 0;
    while ((input_size = (size_t)ReadBuf(in, (char*)input, input_length)) > 0) {
        size_t output_length = 0;
        bool t = my_decoder->decode(input, input_size, output, output_size, output_length);
        out.write((char*)output, output_length);
        while (t) {
            t = my_decoder->flush(output, output_size, output_length);
            out.write((char*)output, output_length);
        }
    }
}

void SetChunkSize(size_t newsize) {
    block_size = newsize;
}
size_t GetChunkSize() {
    return block_size;
}

#endif //HUFFMAN_HUFFMAN_LIBRARY_H