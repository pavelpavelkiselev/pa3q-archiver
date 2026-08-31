/**
 * @file encoder.cpp
 * @brief Encoder class implementation.
 */

#include "encoder.hpp"

Encoder::Encoder(std::ifstream &in, std::ofstream &out) : input(in), output(out) {}

void Encoder::encode(const bool bit) {
    const uint32_t range = static_cast<uint32_t>(high - low);
    const uint32_t prob = static_cast<uint32_t>(predictor.next_bit_probability());
    const uint32_t result = (range * prob) >> 16;
    const uint16_t mid = low + static_cast<uint16_t>(result);

    if (bit) {
        low = mid + 1;
    } else {
        high = mid;
    }

    predictor.update_model(bit);

    while (((low ^ high) >> 8) == 0) {
        /*  While the highest byte of low and high is the same,
            flush the byte to the file. */
        output.put(static_cast<unsigned char>(high >> 8));
        low <<= 8;
        high = (high << 8) | 0xFF;
    }
}

bool Encoder::decode() {
    const uint32_t range = static_cast<uint32_t>(high - low);
    const uint32_t prob = static_cast<uint32_t>(predictor.next_bit_probability());
    const uint32_t result = (range * prob) >> 16;
    const uint16_t mid = low + static_cast<uint16_t>(result);
    const bool bit = (current > mid);

    if (bit) {
        low = mid + 1;
    } else {
        high = mid;
    }

    predictor.update_model(bit);

    while (((low ^ high) >> 8) == 0) {
        low <<= 8;
        high = (high << 8) | 0xFF;
        int c = input.get();
        if (c == EOF) {
            c = 0;
        }
        current = (current << 8) | (c & 0xFF);
    }
    return bit;
}
