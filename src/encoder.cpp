/**
 * @file encoder.cpp
 * @brief Encoder class implementation.
 */

#include "encoder.hpp"

Encoder::Encoder(std::ifstream& in, std::ofstream& out)
    : input(in), output(out) {}

void Encoder::encode(bool bit) {
  const unsigned __int128 range = static_cast<unsigned __int128>(high - low);
  const unsigned __int128 prob =
      static_cast<unsigned __int128>(predictor.next_bit_probability());
  const unsigned __int128 result = (range * prob) >> 64;
  const uint64_t mid = low + static_cast<uint64_t>(result);

  if (bit) {
    low = mid + 1;
  } else {
    high = mid;
  }

  predictor.update_model(bit);

  while (((low ^ high) >> 56) == 0) {
    output.put(static_cast<unsigned char>(high >> 56));
    low <<= 8;
    high = (high << 8) | 0xFF;
  }
}

bool Encoder::decode() {
  const unsigned __int128 range = static_cast<unsigned __int128>(high - low);
  const unsigned __int128 prob =
      static_cast<unsigned __int128>(predictor.next_bit_probability());
  const unsigned __int128 result = (range * prob) >> 64;
  const uint64_t mid = low + static_cast<uint64_t>(result);
  bool bit;

  if (current <= mid) {
    bit = 0;
    high = mid;
  } else {
    bit = 1;
    low = mid + 1;
  }

  predictor.update_model(bit);

  while (((low ^ high) >> 56) == 0) {
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

