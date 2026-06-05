/**
 * @file decompressor.cpp
 * @brief Decompressor class implementation.
 */

#include "decompressor.hpp"
#include <stdexcept>

Decompressor::Decompressor(const std::string& in, const std::string& out)
    : Encoder(input_file, output_file) {
  input_file.open(in, std::ios::binary);
  if (!input_file) {
    throw std::runtime_error("Could not open input file for reading: " + in);
  }

  output_file.open(out, std::ios::binary);
  if (!output_file) {
    throw std::runtime_error("Could not open output file for writing: " + out);
  }

  if (!input_file.read(reinterpret_cast<char*>(&file_size),
                       sizeof(file_size))) {
    throw std::runtime_error("Failed to read file size header from an archive.");
  }

  for (short i = 0; i < 8; i++) {
    int c = input_file.get();
    if (c == EOF) {
      c = 0;
    }
    current = (current << 8) | (c & 0xFF);
  }
}

void Decompressor::decompress() {
  for (uint64_t i = 0; (i < file_size) and (i != 0xFFFFFFFFFFFFFFFF); i++) {
    int c = 1;
    while (c < 0x100) {
      c <<= 1;
      c += decode();
    }
    output_file.put(static_cast<unsigned char>(c & 0xFF));
  }
  input_file.close();
  output_file.close();
}

