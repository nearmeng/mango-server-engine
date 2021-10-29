#pragma once
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
//#include "log/log.h"
#include <cstdio>
#include "shm_log.h"

const auto kShmAddress = reinterpret_cast<void*>(0x000070000000000);

class ShmMapping {
 public:
  static bool Open(key_t key, std::size_t size, bool is_resume, int* out_shm_id) {
    auto shm_id = shm_get(key, size, is_resume);
    if (shm_id == -1) return false;

    *out_shm_id = shm_id;
    return true;
  }

  static bool Close(int shm_id, void* memory) { return shm_rm(shm_id, memory); }

  static void* Mapping(int shm_id, std::size_t size) { return shm_at(shm_id, size); }

 private:
  static int shm_get(key_t key, std::size_t shm_size, bool is_resume) {
    int shm_id = shmget(key, shm_size, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id != -1) {
      return shm_id;
    }

    if (errno != EEXIST) {
      SHMLOG_ERROR( "[shm_get] alloc failed. [errno:%d] %s", errno, strerror(errno));
      return -1;
    }
    else {
      if(!is_resume){
        SHMLOG_ERROR( "[shm_get] alloc failed. shm already exist");
        return -1;
      }
    }

    shm_id = shmget(key, shm_size, 0666);
    if (shm_id != -1) {
      return shm_id;
    }

    shm_id = shmget(key, 0, 0666);
    if (shm_id == -1) {
      SHMLOG_ERROR( "[shm_get] touch failed. [errno:%d] %s\n", errno, strerror(errno));
      return -1;
    }

    if (is_resume) {
      SHMLOG_ERROR( "[shm_get] shm already exist, but size not match, alloc failed.\n");
      return -1;
    }

    if (shmctl(shm_id, IPC_RMID, nullptr) == -1) {
      SHMLOG_ERROR( "[shm_get] rm failed. [errno:%d] %s\n", errno, strerror(errno));
      return -1;
    }

    shm_id = shmget(key, shm_size, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1) {
      SHMLOG_ERROR( "[shm_get] alloc again failed. [errno:%d] %s\n", errno, strerror(errno));
      return -1;
    }
    return shm_id;
  }

  static void* shm_at(int shm_id, std::size_t size) {
    struct shmid_ds sd_ds;
    if (shmctl(shm_id, IPC_STAT, &sd_ds) == -1) {
      SHMLOG_ERROR( "[shm_at] shmctl stat failed. [errno:%d] %s\n", errno, strerror(errno));
      return nullptr;
    }

    if (sd_ds.shm_segsz != size) {
      SHMLOG_ERROR( "[shm_at] invalid seg size. shm_segsz=%u, size=%u\n", sd_ds.shm_segsz, size);
      return nullptr;
    }

    void* ptr = shmat(shm_id, kShmAddress, 0);
    if (ptr == reinterpret_cast<void*>(-1)) {
      SHMLOG_ERROR( "[shm_at] shmat failed. [errno:%d] %s\n", errno, strerror(errno));
      return nullptr;
    }
    return ptr;
  }

  static bool shm_rm(int shm_id, void* memory) {
    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
      SHMLOG_ERROR( "[shm_rm] shmctl rm failed. [errno:%d] %s\n", errno, strerror(errno));
      return false;
    }
    if (shmdt(memory) == -1) {
      SHMLOG_ERROR( "[shm_rm] shmdt failed. [errno:%d] %s\n", errno, strerror(errno));
    }
    return true;
  }
};
