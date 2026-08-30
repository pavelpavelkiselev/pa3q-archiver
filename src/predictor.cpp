/**
 * @file predictor.cpp
 * @brief Predictor class implementation.
 */

#include "predictor.hpp"

uint32_t Predictor::next_bit_probability() {
    p0_order0 =
        static_cast<double>(count0[context][0] + 1) / (count0[context][0] + count0[context][1] + 2);
    p0_order1 = static_cast<double>(count1[prev1][context][0] + 1) /
                (count1[prev1][context][0] + count1[prev1][context][1] + 2);
    p0_order2 = static_cast<double>(count2[prev2][context][0] + 1) /
                (count2[prev2][context][0] + count2[prev2][context][1] + 2);

    s0 = stretch(clamp_p(p0_order0));
    s1 = stretch(clamp_p(p0_order1));
    s2 = stretch(clamp_p(p0_order2));

    x = w0 * s0 + w1 * s1 + w2 * s2;
    mixed_p0 = squash(x);

    if (mixed_p0 < 0.0000000001) {
        mixed_p0 = 0.0000000001;
    }
    if (mixed_p0 > 0.9999999999) {
        mixed_p0 = 0.9999999999;
    }

    return static_cast<uint32_t>(mixed_p0 * 4294967295.0);
}

void Predictor::update_model(const bool bit) {
    const double y_0 = ((bit == false) ? (1.0) : (0.0));
    const double error = y_0 - mixed_p0;

    w0 += learning_rate * error * s0;
    w1 += learning_rate * error * s1;
    w2 += learning_rate * error * s2;

    w0 = clamp_w(w0);
    w1 = clamp_w(w1);
    w2 = clamp_w(w2);

    count0[context][bit] += 1;
    if ((count0[context][0] + count0[context][1] + 2) >= 0xFFFFFFFF) {
        count0[context][0] /= 2;
        count0[context][1] /= 2;
    }

    count1[prev1][context][bit] += 1;
    if ((count1[prev1][context][0] + count1[prev1][context][1] + 2) >= 0xFFFFFFFF) {
        count1[prev1][context][0] /= 2;
        count1[prev1][context][1] /= 2;
    }

    count2[prev2][context][bit] += 1;
    if ((count2[prev2][context][0] + count2[prev2][context][1] + 2) >= 0xFFFFFFFF) {
        count2[prev2][context][0] /= 2;
        count2[prev2][context][1] /= 2;
    }

    context = (context << 1) + bit;
    if (context >= 0x100) {
        context &= 0xFF;
        prev2 = (prev2 << 8) | prev1;
        prev1 = context;
        context = 1;
    }
}
