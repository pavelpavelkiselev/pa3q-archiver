/**
 * @file predictor.hpp
 * @brief Predictor class declaration.
 */

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

static_assert(std::numeric_limits<double>::is_iec559,
              "ERROR: IEEE-754 floating-point not supported!");

static_assert(sizeof(double) == 8, "ERROR: Wrong size of a type 'double'!");

static_assert(std::numeric_limits<double>::radix == 2,
              "ERROR: Base of floating-point types must be 2!");

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
    uint16_t context = 1;
    uint8_t prev1 = 0;
    uint16_t prev2 = 0;

    std::array<std::array<uint16_t, 2>, 256> count0 = {};
    std::vector<std::array<std::array<uint16_t, 2>, 256>> count1 =
        std::vector<std::array<std::array<uint16_t, 2>, 256>>(256);
    std::vector<std::array<std::array<uint16_t, 2>, 256>> count2 =
        std::vector<std::array<std::array<uint16_t, 2>, 256>>(65536);

    // Raw probabilities
    double p0_order0 = 0.5;
    double p0_order1 = 0.5;
    double p0_order2 = 0.5;

    // Stretched probabilities
    double s0 = 0.0;
    double s1 = 0.0;
    double s2 = 0.0;

    // Unsquashed mixed probability
    double x = 0.0;

    // Final mixed probability
    double mixed_p0 = 0.0;

    static constexpr double learning_rate = 0.00390625; // 2^(-8)

    // Contexts' weights
    double w0 = 1.0;
    double w1 = 1.0;
    double w2 = 1.0;

    /**
     * @brief Converts a probability to logistic space (log-odds).
     * @param p Probability value in range (0, 1).
     * @return Log-odds representation.
     */
    inline double stretch(const double p) const { return std::log(p / (1.0 - p)); }

    /**
     * @brief Converts a logistic value back to a probability (Sigmoid function).
     * @param x Logistic value.
     * @return Probability in range (0, 1).
     */
    inline double squash(const double x) const { return (1.0 / (1.0 + std::exp(-x))); }

    /**
     * @brief Clamps the probability to avoid limits approaching 0 or 1.
     * @param p Original probability.
     * @return Clamped probability.
     */
    inline double clamp_p(const double p) const {
        if (p < 0.00000004) {
            return 0.00000004;
        } else if (p > 0.99999996) {
            return 0.99999996;
        } else {
            return p;
        }
    }

    /**
     * @brief Clamps the weight to avoid -inf to inf range.
     * @param w Original weight.
     * @return Clamped weight.
     */
    inline double clamp_w(const double w) const {
        if (w < -32.0) {
            return -32.0;
        }
        if (w > 32.0) {
            return 32.0;
        }
        return w;
    };

    /**
     * @brief Clamps the mixed probability to avoid limits approaching 0 or 1.
     * @param m Original probability.
     * @return Clamped probability.
     */
    inline double clamp_m(const double m) const {
        if (m < 0.0000000001) {
            return 0.0000000001;
        } else if (m > 0.9999999999) {
            return 0.9999999999;
        } else {
            return m;
        }
    }

public:
    /**
     * @brief Predicts the probability that the next bit will be 0.
     * @return A scaled integer probability at [0; 2^16).
     */
    uint16_t next_bit_probability();

    /**
     * @brief Updates the context models and neural network weights.
     * @param bit The actual bit that was exctracted.
     */
    void update_model(const bool bit);
};