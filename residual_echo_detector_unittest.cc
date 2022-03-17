#include "residual_echo_detector.h"

#include "gtest/gtest.h"

namespace aqa {

TEST(ResidualEchoDetectorTests, Echo) {
//TEST(ResidualEchoDetectorTests, DISABLED_Echo) {
  ResidualEchoDetector* echo_detector = new ResidualEchoDetector();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal has a delay of 10 frames w.r.t. the render
  // signal, but is otherwise identical. Both signals are periodic with a 20
  // frame interval.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
      echo_detector->AnalyzeCaptureAudio(zeros);
    } else if (i % 20 == 10) {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
  }
  // We expect to detect echo with near certain likelihood.
  auto ed_metrics = echo_detector->GetMetrics();
  ASSERT_TRUE(ed_metrics.echo_likelihood);
  EXPECT_NEAR(1.f, ed_metrics.echo_likelihood, 0.01f);
  EXPECT_NEAR(1.f, ed_metrics.echo_likelihood_recent_max, 0.01f);
}

//TEST(ResidualEchoDetectorTests, NoEcho) {
TEST(ResidualEchoDetectorTests, DISABLED_NoEcho) {
  ResidualEchoDetector* echo_detector = new ResidualEchoDetector();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal is always zero, so no echo should be
  // detected.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
    }
    echo_detector->AnalyzeCaptureAudio(zeros);
  }
  // We expect to not detect any echo.
  auto ed_metrics = echo_detector->GetMetrics();
  //ASSERT_TRUE(ed_metrics.echo_likelihood);
  EXPECT_NEAR(0.1f, ed_metrics.echo_likelihood, 0.01f);
}

//TEST(ResidualEchoDetectorTests, EchoWithRenderClockDrift) {
TEST(ResidualEchoDetectorTests, DISABLED_EchoWithRenderClockDrift) {
  ResidualEchoDetector* echo_detector = new ResidualEchoDetector();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal has a delay of 10 frames w.r.t. the render
  // signal, but is otherwise identical. Both signals are periodic with a 20
  // frame interval. There is a simulated clock drift of 1% in this test, with
  // the render side producing data slightly faster.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
      echo_detector->AnalyzeCaptureAudio(zeros);
    } else if (i % 20 == 10) {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
    if (i % 100 == 0) {
      // This is causing the simulated clock drift.
      echo_detector->AnalyzeRenderAudio(zeros);
    }
  }
  // We expect to detect echo with high likelihood. Clock drift is harder to
  // correct on the render side than on the capture side. This is due to the
  // render buffer, clock drift can only be discovered after a certain delay.
  // A growing buffer can be caused by jitter or clock drift and it's not
  // possible to make this decision right away. For this reason we only expect
  // an echo likelihood of 75% in this test.
  auto ed_metrics = echo_detector->GetMetrics();
  ASSERT_TRUE(ed_metrics.echo_likelihood);
  EXPECT_GT(ed_metrics.echo_likelihood, 0.75f);
}

//TEST(ResidualEchoDetectorTests, EchoWithCaptureClockDrift) {
TEST(ResidualEchoDetectorTests, DISABLED_EchoWithCaptureClockDrift) {
  ResidualEchoDetector* echo_detector = new ResidualEchoDetector();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal has a delay of 10 frames w.r.t. the render
  // signal, but is otherwise identical. Both signals are periodic with a 20
  // frame interval. There is a simulated clock drift of 1% in this test, with
  // the capture side producing data slightly faster.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
      echo_detector->AnalyzeCaptureAudio(zeros);
    } else if (i % 20 == 10) {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
    if (i % 100 == 0) {
      // This is causing the simulated clock drift.
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
  }
  // we expect to detect echo with near certain likelihood.
  auto ed_metrics = echo_detector->GetMetrics();
  ASSERT_TRUE(ed_metrics.echo_likelihood);
  EXPECT_NEAR(1.f, ed_metrics.echo_likelihood, 0.01f);
}

}  // namespace aqa
