/**
 * @file predictor.hpp
 * @brief Predictor class declaration.
 */

#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "arithmetic.h"

/**
 * @class Predictor
 * @brief Estimates the probability of the next bit being 0 or 1.
 *
 * The Predictor uses 3 context models (order-0, order-1, order-2) and mixes
 * their predictions using a stretch/squash logistic regression model updated
 * dynamically via gradient descent.
 */
class Predictor {
private:
    uint16_t current_context = 1;
    uint8_t context_o1 = 0;
    uint16_t context_o2 = 0;

    std::array<std::array<uint16_t, 2>, 256> count_o0 = {};

    std::vector<std::array<std::array<uint16_t, 2>, 256>> count_o1 =
        std::vector<std::array<std::array<uint16_t, 2>, 256>>(256);

    std::vector<std::array<std::array<uint16_t, 2>, 256>> count_o2 =
        std::vector<std::array<std::array<uint16_t, 2>, 256>>(65536);

    // Raw probabilities. 2^16 scale. From [0; 1) to [0; 65536)
    uint16_t prob_o0 = 32768; // 0.5 at 2^16 scale.
    uint16_t prob_o1 = 32768;
    uint16_t prob_o2 = 32768;

    // Stretched probabilities. 2^27 scale. From [-12; 12] to [-1610612736; 1610612736]
    int32_t stretched_o0 = 0;
    int32_t stretched_o1 = 0;
    int32_t stretched_o2 = 0;

    // Unsquashed mixed probability. 2^27 scale (same as stretched probabilities).
    int64_t mixed_stretched = 0;

    // Final mixed probability. 2^16 scale.
    uint16_t final_prob = 32768; // 0.5 at 2^16 scale.

    static constexpr int64_t learning_rate_bits = 8; // learning rate = 1/256

    // Contexts' weights. 2^40 scale. From [-48; 48] to [-52776558133248; 52776558133248]
    int64_t weight_o0 = 1099511627776; // 1.0 at 40-bit scale.
    int64_t weight_o1 = 1099511627776;
    int64_t weight_o2 = 1099511627776;

public:
    /**
     * @brief Predicts the probability that the next bit will be 0.
     * @return A scaled integer probability at [0; 2^16).
     */
    uint16_t next_bit_probability();

    /**
     * @brief Updates the context models and neural network weights.
     * @param bit The actual bit that was extracted.
     */
    void update_model(const bool bit);
};
