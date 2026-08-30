/**
 * @file compressor.cpp
 * @brief Compressor class implementation.
 */

#include "compressor.hpp"
#include <filesystem>
#include <stdexcept>

Compressor::Compressor(const std::string &in, const std::string &out)
    : Encoder(input_file, output_file) {
    input_file.open(in, std::ios::binary);
    if (!input_file) {
        throw std::runtime_error("Could not open input file for reading: " + in);
    }

    output_file.open(out, std::ios::binary);
    if (!output_file) {
        throw std::runtime_error("Could not open output file for writing: " + out);
    }

    if (std::filesystem::file_size(in) > 0xFFFFFFFFFFFFFFFF) {
        throw std::runtime_error("Input file's size is too large: (2^64)-1 bytes max.");
    }

    try {
        file_size = std::filesystem::file_size(in);
    } catch (const std::filesystem::filesystem_error &e) {
        throw std::runtime_error("Error retrieving file size: " + std::string(e.what()));
    }

    output_file.write(reinterpret_cast<const char *>(&file_size), sizeof(file_size));
    if (!output_file) {
        throw std::runtime_error("Failed to write file size header to an archive.");
    }
}

void Compressor::compress() {
    int c;
    while ((c = input_file.get()) != EOF) {
        for (int i = 7; i >= 0; i--) {
            const bool current_bit = ((c >> i) & 1);
            encode(current_bit);
        }
    }

    while (((low ^ high) & ((static_cast<uint32_t>(0xFF)) << 24)) == 0) {
        output_file.put(static_cast<unsigned char>(high >> 24));
        low <<= 8;
        high = (high << 8) | 0xFF;
    }

    output_file.put(static_cast<unsigned char>(high >> 24));
    input_file.close();
    output_file.close();
}
