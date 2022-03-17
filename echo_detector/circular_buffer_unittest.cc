#include "echo_detector/circular_buffer.h"

#include "gtest/gtest.h"

namespace aqa {

TEST(CircularBufferTests, LessThanMaxTest) {
  CircularBuffer test_buffer(3);

  test_buffer.Push(1.f);
  EXPECT_EQ(1, test_buffer.Size());

  test_buffer.Push(2.f);
  EXPECT_EQ(2, test_buffer.Size());

  EXPECT_EQ(1.f, test_buffer.Pop());
  EXPECT_EQ(1, test_buffer.Size());

  EXPECT_EQ(2.f, test_buffer.Pop());
  EXPECT_EQ(0, test_buffer.Size());
}

TEST(CircularBufferTests, FillTest) {
  CircularBuffer test_buffer(3);

  test_buffer.Push(1.f);
   EXPECT_EQ(1, test_buffer.Size());

  test_buffer.Push(2.f);
  EXPECT_EQ(2, test_buffer.Size());

  test_buffer.Push(3.f);
  EXPECT_EQ(3, test_buffer.Size());

  EXPECT_EQ(1.f, test_buffer.Pop());
  EXPECT_EQ(2, test_buffer.Size());

  EXPECT_EQ(2.f, test_buffer.Pop());
  EXPECT_EQ(1, test_buffer.Size());

  EXPECT_EQ(3.f, test_buffer.Pop());
  EXPECT_EQ(0, test_buffer.Size());
}


TEST(CircularBufferTests, OverflowTest) {
  CircularBuffer test_buffer(3);

  test_buffer.Push(1.f);
  test_buffer.Push(2.f);
  test_buffer.Push(3.f);
  test_buffer.Push(4.f);

  EXPECT_EQ(2.f, test_buffer.Pop());
  EXPECT_EQ(3.f, test_buffer.Pop());
  EXPECT_EQ(4.f, test_buffer.Pop());
}

TEST(CircularBufferTests, ReadFromEmpty) {
  CircularBuffer test_buffer(3);
  EXPECT_EQ(std::nullopt, test_buffer.Pop());
}

}  // namespace aqa

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
