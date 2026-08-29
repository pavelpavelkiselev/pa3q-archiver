/**
 * @file encoder.hpp
 * @brief Encoder base class declaration.
 */

#pragma once

#include "predictor.hpp"
#include <cstdint>
#include <fstream>

/**
 * @class Encoder
 * @brief Handles the arithmetic (range) encoding and decoding.
 */
class Encoder {
protected:
    Predictor predictor;
    uint32_t low = 0;
    uint32_t high = 0xFFFFFFFF;
    uint32_t current = 0;
    std::ifstream &input;
    std::ofstream &output;

public:
    /**
     * @brief Constructs an Encoder.
     * @param in Reference to the input file stream.
     * @param out Reference to the output file stream.
     */
    Encoder(std::ifstream &in, std::ofstream &out);

    /**
     * @brief Encodes a single bit into the output stream.
     * @param bit The bit to encode to a an output file.
     */
    void encode(const bool bit);

    /**
     * @brief Decodes a single bit from the input file.
     * @return The decoded bool bit.
     */
    bool decode();
};
