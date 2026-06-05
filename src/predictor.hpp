/**
 * @file predictor.hpp
 * @brief Predictor class declaration.
 */

#pragma once

#include <cmath>
#include <cstdint>
#include <vector>

/**
 * @class Predictor
 * @brief Estimates the probability of the next bit being 0 or 1.
 *
 * The Predictor uses 3 context models (order-0, order-1, Order-2) and mixes
 * their predictions using a stretch/squash logistic regression model updated
 * dynamically via gradient descent.
 */
class Predictor {
private:
  uint16_t context = 1;
  uint8_t prev1 = 0;
  uint16_t prev2 = 0;

  long double w0 = 1.0;
  long double w1 = 1.0;
  long double w2 = 1.0;

  uint64_t count0[256][2] = {0};

  std::vector<std::vector<std::vector<uint64_t>>> count1 =
      std::vector<std::vector<std::vector<uint64_t>>>(
          256, std::vector<std::vector<uint64_t>>(
                   256, std::vector<uint64_t>(2, 0)));

  std::vector<std::vector<std::vector<uint64_t>>> count2 =
      std::vector<std::vector<std::vector<uint64_t>>>(
          65536, std::vector<std::vector<uint64_t>>(
                     256, std::vector<uint64_t>(2, 0)));

  /**
   * @brief Converts a probability to logistic space (log-odds).
   * @param p Probability value in range (0, 1).
   * @return Log-odds representation.
   */
  inline long double stretch(long double p) const {
    return std::log(p / (1.0 - p));
  }

  /**
   * @brief Converts a logistic value back to a probability (Sigmoid function).
   * @param x Logistic value.
   * @return Probability in range (0, 1).
   */
  inline long double squash(long double x) const {
    return 1.0 / (1.0 + std::exp(-x));
  }

  /**
   * @brief Clamps the probability to avoid limits approaching 0 or 1.
   * @param p Original probability.
   * @return Clamped probability.
   */
  inline long double clamp_p(long double p) const {
    if (p < 0.00005) {
      return 0.00005;
    } else if (p > 0.99995) {
      return 0.99995;
    } else {
      return p;
    }
  }

public:
  /**
   * @brief Predicts the probability that the next bit will be 0.
   * @return A scaled integer probability at [0; 2^64).
   */
  uint64_t next_bit_probability() const;

  /**
   * @brief Updates the context models and neural network weights.
   * @param bit The actual bit that was exctracted.
   */
  void update_model(bool bit);
};

