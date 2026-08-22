/**
 * @file compressor.hpp
 * @brief Compressor class declaration.
 */

#pragma once

#include <string>
#include "encoder.hpp"

/**
 * @class Compressor
 * @brief Manages the compression mode of a file, using Encoder.
 */
class Compressor : public Encoder {
private:
  std::ifstream input_file;
  std::ofstream output_file;
  uint64_t file_size = 0;

public:
  /**
   * @brief Initializes streams and prepares the file for compression.
   * @param in Path to the input file.
   * @param out Path to the output file.
   * @throws std::runtime_error If files cannot be opened.
   */
  Compressor(const std::string& in, const std::string& out);

  /**
   * @brief Compresses the entire file by bits and then flushes ranges.
   */
  void compress();
};

