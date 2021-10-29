#include "resuming_guard.h"

static bool g_is_shm_resuming_flag = false;

bool ShmIsResuming() { return g_is_shm_resuming_flag; }

ResumingGuard::ResumingGuard() { g_is_shm_resuming_flag = true; }

ResumingGuard::~ResumingGuard() { g_is_shm_resuming_flag = false; }
