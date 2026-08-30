/**
 * @file encoder.cpp
 * @brief Encoder class implementation.
 */

#include "encoder.hpp"

Encoder::Encoder(std::ifstream &in, std::ofstream &out) : input(in), output(out) {}

void Encoder::encode(const bool bit) {
    const uint64_t range = static_cast<uint64_t>(high - low);
    const uint64_t prob = static_cast<uint64_t>(predictor.next_bit_probability());
    const uint64_t result = (range * prob) >> 32;
    const uint32_t mid = low + static_cast<uint32_t>(result);

    if (bit) {
        low = mid + 1;
    } else {
        high = mid;
    }

    predictor.update_model(bit);

    while (((low ^ high) >> 24) == 0) {
        /*  While the highest byte of low and high is the same,
            flush the byte to the file. */
        output.put(static_cast<unsigned char>(high >> 24));
        low <<= 8;
        high = (high << 8) | 0xFF;
    }
}

bool Encoder::decode() {
    const uint64_t range = static_cast<uint64_t>(high - low);
    const uint64_t prob = static_cast<uint64_t>(predictor.next_bit_probability());
    const uint64_t result = (range * prob) >> 32;
    const uint32_t mid = low + static_cast<uint32_t>(result);
    const bool bit = (current > mid);

    if (bit) {
        low = mid + 1;
    } else {
        high = mid;
    }

    predictor.update_model(bit);

    while (((low ^ high) >> 24) == 0) {
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
