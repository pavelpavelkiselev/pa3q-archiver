/**
 * @file predictor.cpp
 * @brief Predictor class implementation.
 */

#include "predictor.hpp"

uint16_t Predictor::next_bit_probability() {
    p0_order0 = static_cast<uint16_t>(((static_cast<uint32_t>(count0[context][0]) + 1) << 16) / 
        (static_cast<uint32_t>(count0[context][0]) + static_cast<uint32_t>(count0[context][1]) + 2));

    p0_order1 = static_cast<uint16_t>(((static_cast<uint32_t>(count1[prev1][context][0]) + 1) << 16) / 
        (static_cast<uint32_t>(count1[prev1][context][0]) + static_cast<uint32_t>(count1[prev1][context][1]) + 2));
    
    p0_order2 = static_cast<uint16_t>(((static_cast<uint32_t>(count2[prev2][context][0]) + 1) << 16) / 
        (static_cast<uint32_t>(count2[prev2][context][0]) + static_cast<uint32_t>(count2[prev2][context][1]) + 2));

    s0 = stretch(p0_order0);
    s1 = stretch(p0_order1);
    s2 = stretch(p0_order2);

    // 32768 = 2^15, difference between 2^40 and 2^25 weights' scales
    mx = ((w0 / 32768) * (static_cast<int64_t>(s0)) + 
        (w1 / 32768) * (static_cast<int64_t>(s1)) + 
        (w2 / 32768) * (static_cast<int64_t>(s2))) / 33554432; // 3355432 = 2^25
    // weights' scale is 2^25, not 2^27, because weights' range is [-48; 48], not [-12; 12]
    // Every multiplying number is in range [-1610612736; 1610612736], no int64_t overflow.

    mixed_p0 = squash(clamp_s(mx));

    return mixed_p0;
}

void Predictor::update_model(const bool bit) {
    const int64_t y_0 = ((bit == false) ? (65536) : (0)); // 16 bit scale.
    const int64_t error = y_0 - mixed_p0;

    // 8192 = 2^13; difference in scale between weights and stretched probabilities (2^40 and 2^27)
    // 16777216 = 2^16 * 2^8; error's scale and learning_rate
    w0 = clamp_w(w0 + clamp_w(((static_cast<int64_t>(s0) * 8192) * error) / 16777216));
    w1 = clamp_w(w1 + clamp_w(((static_cast<int64_t>(s1) * 8192) * error) / 16777216));
    w2 = clamp_w(w2 + clamp_w(((static_cast<int64_t>(s2) * 8192) * error) / 16777216));

    // Contexts' count halving.
    count0[context][bit] += 1;
    if ((count0[context][0] + count0[context][1] + 2) >= 0xFFFF) {
        count0[context][0] /= 2;
        count0[context][1] /= 2;
    }

    count1[prev1][context][bit] += 1;
    if ((count1[prev1][context][0] + count1[prev1][context][1] + 2) >= 0xFFFF) {
        count1[prev1][context][0] /= 2;
        count1[prev1][context][1] /= 2;
    }

    count2[prev2][context][bit] += 1;
    if ((count2[prev2][context][0] + count2[prev2][context][1] + 2) >= 0xFFFF) {
        count2[prev2][context][0] /= 2;
        count2[prev2][context][1] /= 2;
    }

    context = (context << 1) + bit;
    if (context >= 0x100) {
        context &= 0xFF;
        prev2 = (prev2 << 8) | prev1;
        prev1 = static_cast<uint8_t>(context);
        context = 1;
    }
}