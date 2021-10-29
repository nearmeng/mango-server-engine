#pragma once
#include <math.h>

namespace gw {

struct MathUtility {
  static const float kFloatTolerance;
  static const double kDoubleTolerance;

  template <typename T>
  static T Abs(T t) {
    return t >= static_cast<T>(0) ? t : -t;
  }

  static float Abs(float f) {
    return ::fabsf(f);
  }

  static double Abs(double d) {
    return ::fabs(d);
  }

  template <typename T>
  static bool IsEqual(T a, T b) {
    return a == b;
  }

  static bool IsEqual(float a, float b) {
    return MathUtility::Abs(a - b) <= MathUtility::kFloatTolerance;
  }

  static bool IsEqual(double a, double b) {
    return MathUtility::Abs(a - b) <= MathUtility::kDoubleTolerance;
  }

  static bool IsMore(float a, float b) {
    return a - b > MathUtility::kFloatTolerance;
  }

  static bool IsMoreEqu(float a, float b) {
    return a - b > -MathUtility::kFloatTolerance;
  }

  template <typename T>
  static bool IsPowerOfTwo(T t) {
    return (t & (t - 1)) == static_cast<T>(0);
  }

  template <typename T>
  static inline T Clamp(T minv, T value, T maxv) {
    return value < minv ? minv : (value > maxv ? maxv : value);
  }

  /**
     * Helper function for random implementations.
     *
     * @param min The lower bound of the range.
     * @param max The upper bound of the range.
     * @return A random number in [min, max].
     */
  static int RandRange(int min, int max);

  static bool RandBool() {
    return RandRange(0, 1) == 1;
  }

  static bool RandProbabilityHit(int prob, int prob_max) {
    return RandRange(1, prob_max) <= prob;
  }

  static bool RandPercentageHit(int percentage) {
    return RandProbabilityHit(percentage, 100);
  }
};

}  // namespace gw
