/**
 * @file decompressor.hpp
 * @brief Decompressor class declaration.
 */

#pragma once

#include <string>
#include "encoder.hpp"

/**
 * @class Decompressor
 * @brief Derived from Encoder, manages the decompression of an archive.
 */
class Decompressor : public Encoder {
private:
  std::ifstream input_file;
  std::ofstream output_file;
  uint64_t file_size = 0;

public:
  /**
   * @brief Initializes streams and reads the size of an original file.
   * @param in Path to the compressed input file.
   * @param out Path to the output decompressed file.
   * @throws std::runtime_error If files cannot be opened.
   */
  Decompressor(const std::string& in, const std::string& out);

  /**
   * @brief Decompresses previously compressed file.
   */
  void decompress();
};

