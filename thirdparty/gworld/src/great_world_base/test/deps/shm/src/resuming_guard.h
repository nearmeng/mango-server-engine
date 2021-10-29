#pragma once

struct ResumingGuard final {
  explicit ResumingGuard();
  ~ResumingGuard();
  ResumingGuard(const ResumingGuard&) = delete;
  ResumingGuard(ResumingGuard&&) = delete;
  ResumingGuard& operator=(const ResumingGuard&) = delete;
  ResumingGuard& operator=(ResumingGuard&&) = delete;
};

bool ShmIsResuming();
