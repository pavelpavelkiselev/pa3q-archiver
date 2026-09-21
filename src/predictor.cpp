/**
 * @file predictor.cpp
 * @brief Predictor class implementation.
 */

#include "predictor.hpp"

uint16_t Predictor::next_bit_probability() {
    prob_o0 =
        static_cast<uint16_t>(((static_cast<uint32_t>(count_o0[current_context][0]) + 1) << 16) /
                              (static_cast<uint32_t>(count_o0[current_context][0]) +
                               static_cast<uint32_t>(count_o0[current_context][1]) + 2));

    prob_o1 = static_cast<uint16_t>(
        ((static_cast<uint32_t>(count_o1[context_o1][current_context][0]) + 1) << 16) /
        (static_cast<uint32_t>(count_o1[context_o1][current_context][0]) +
         static_cast<uint32_t>(count_o1[context_o1][current_context][1]) + 2));

    prob_o2 = static_cast<uint16_t>(
        ((static_cast<uint32_t>(count_o2[context_o2][current_context][0]) + 1) << 16) /
        (static_cast<uint32_t>(count_o2[context_o2][current_context][0]) +
         static_cast<uint32_t>(count_o2[context_o2][current_context][1]) + 2));

    stretched_o0 = stretch(prob_o0);
    stretched_o1 = stretch(prob_o1);
    stretched_o2 = stretch(prob_o2);

    mixed_stretched = ((weight_o0 >> 15) * (static_cast<int64_t>(stretched_o0)) +
                       (weight_o1 >> 15) * (static_cast<int64_t>(stretched_o1)) +
                       (weight_o2 >> 15) * (static_cast<int64_t>(stretched_o2))) >> 25;
    // Weights are firstly scaled from 2^40 to 2^25 to prevent int64_t overflow.

    final_prob = squash(clamp_s(mixed_stretched));

    return final_prob;
}

void Predictor::update_model(const bool bit) {
    const int64_t y_0 = ((bit == false) ? (65536) : (0)); // 16 bit scale.
    const int64_t error = y_0 - final_prob;

    // 2^13 - scale difference between weights (2^40) and stretched probabilities (2^40)
    weight_o0 = clamp_w(weight_o0 + clamp_w(((static_cast<int64_t>(stretched_o0) << 13) * error) >>
                                            (16 + learning_rate_bits)));
    weight_o1 = clamp_w(weight_o1 + clamp_w(((static_cast<int64_t>(stretched_o1) << 13) * error) >>
                                            (16 + learning_rate_bits)));
    weight_o2 = clamp_w(weight_o2 + clamp_w(((static_cast<int64_t>(stretched_o2) << 13) * error) >>
                                            (16 + learning_rate_bits)));

    // Update context counts
    count_o0[current_context][bit] += 1;
    count_o1[context_o1][current_context][bit] += 1;
    count_o2[context_o2][current_context][bit] += 1;

    // Halving context count
    if ((count_o0[current_context][0] + count_o0[current_context][1] + 2) >= 0xFFFF) {
        count_o0[current_context][0] /= 2;
        count_o0[current_context][1] /= 2;
    }

    if ((count_o1[context_o1][current_context][0] + count_o1[context_o1][current_context][1] + 2) >=
        0xFFFF) {
        count_o1[context_o1][current_context][0] /= 2;
        count_o1[context_o1][current_context][1] /= 2;
    }

    if ((count_o2[context_o2][current_context][0] + count_o2[context_o2][current_context][1] + 2) >=
        0xFFFF) {
        count_o2[context_o2][current_context][0] /= 2;
        count_o2[context_o2][current_context][1] /= 2;
    }

    current_context = (current_context << 1) + bit;
    if (current_context >= 0x100) {
        current_context &= 0xFF;
        context_o2 = (context_o2 << 8) | context_o1;
        context_o1 = static_cast<uint8_t>(current_context);
        current_context = 1;
    }
}