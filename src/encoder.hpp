/**
 * @file encoder.hpp
 * @brief Encoder base class declaration.
 */

#pragma once

#include <cstdint>
#include <fstream>
#include "predictor.hpp"

/**
 * @class Encoder
 * @brief Handles the arithmetic (range) encoding and decoding.
 */
class Encoder {
protected:
  Predictor predictor;
  uint64_t low = 0;
  uint64_t high = 0xFFFFFFFFFFFFFFFF;
  uint64_t current = 0;
  std::ifstream& input;
  std::ofstream& output;

public:
  /**
   * @brief Constructs an Encoder.
   * @param in Reference to the input file stream.
   * @param out Reference to the output file stream.
   */
  Encoder(std::ifstream& in, std::ofstream& out);

  /**
   * @brief Encodes a single bit into the output stream.
   * @param bit The bit to encode to a an output file.
   */
  void encode(bool bit);

  /**
   * @brief Decodes a single bit from the input file.
   * @return The decoded bool bit.
   */
  bool decode();
};

