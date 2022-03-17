#include "echo_detector/moving_max.h"

#include "gtest/gtest.h"

namespace aqa {

TEST(MovingMaxTests, SimpleTest) {
  MovingMax test_moving_max(5);
  test_moving_max.Update(1.0f);
  test_moving_max.Update(1.1f);
  test_moving_max.Update(1.9f);
  test_moving_max.Update(1.87f);
  test_moving_max.Update(1.89f);

  EXPECT_EQ(1.9f, test_moving_max.max());
}


TEST(MovingMaxTests, SlidingWindowTest) {
  MovingMax test_moving_max(5);
  test_moving_max.Update(1.0f);
  test_moving_max.Update(1.9f);
  test_moving_max.Update(1.7f);
  test_moving_max.Update(1.87f);
  test_moving_max.Update(1.89f);
  test_moving_max.Update(1.3f);
  test_moving_max.Update(1.2f);

  EXPECT_LT(test_moving_max.max(), 1.9f);
  EXPECT_NEAR(test_moving_max.max(), 1.9f*0.99, 0.000001);
}

TEST(MovingMaxTests, ClearTest) {
  MovingMax test_moving_max(5);
  test_moving_max.Update(1.0f);
  test_moving_max.Update(1.1f);
  test_moving_max.Update(1.9f);
  test_moving_max.Update(1.87f);
  test_moving_max.Update(1.89f);

  EXPECT_EQ(1.9f, test_moving_max.max());
  test_moving_max.Clear();
  EXPECT_EQ(0.f, test_moving_max.max());
}

TEST(MovingMaxTests, DecayTest) {
  MovingMax test_moving_max(1);
  test_moving_max.Update(1.0f);
  float previous_value = 1.0f;
  // kDecayFactor how fast (counter_ * Ts) the estimated maximum decays after the 
  // previous maximum is no longer valid.
  // the maximum will decay to 1% of its former value after 459 updates.
  // 459 = ceil(log(0.01)/log(0.99))
  for (size_t i = 0; i < 459; ++i) {
    test_moving_max.Update(0.0f);
    EXPECT_LT(test_moving_max.max(), previous_value);
    EXPECT_GT(test_moving_max.max(), 0.0f);
    previous_value = test_moving_max.max();
  }
  EXPECT_LT(test_moving_max.max(), 0.01f);
}

}  // namespace aqa
