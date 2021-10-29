#pragma once

#include <Windows.h>
#include <cstdlib>
#include <string>
// #include "log/log.h"
#include <cstdio>
#include "shm.h"
#include "shm_log.h"

#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#pragma warning(disable : 4312)

const auto kShmAddress = reinterpret_cast<void*>(0x000070000000000);

class ShmMapping {
 public:
  static bool Open(key_t key, std::size_t size, bool is_resume, int* out_shm_id) {
    auto file_mapping = ShmOpen(key, size, is_resume);
    if (file_mapping == nullptr) {
      return false;
    }
    *out_shm_id = reinterpret_cast<int>(file_mapping);
    return true;
  }

  static bool Close(int shm_id, void* memory) {
    auto mapping_handle = reinterpret_cast<HANDLE>(shm_id);
    if (mapping_handle != INVALID_HANDLE_VALUE) {
      UnmapViewOfFile(memory);
      CloseHandle(mapping_handle);
    }
    return true;
  }

  static void* Mapping(int shm_id, std::size_t size) {
    auto mapping_handle = reinterpret_cast<HANDLE>(shm_id);
    if (mapping_handle == INVALID_HANDLE_VALUE) {
      return nullptr;
    }
    auto size_high = static_cast<DWORD>(size >> 32);
    auto size_low = static_cast<DWORD>(size & 0xffffffff);
    auto* base_address =
        MapViewOfFileEx(mapping_handle, FILE_MAP_ALL_ACCESS, 0, size_high, size_low, kShmAddress);
    return base_address;
  }

 private:
  static HANDLE ShmOpen(key_t key, std::size_t size, bool is_resume) {
    auto mapping_name = std::to_string(key);

    auto file_mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, mapping_name.c_str());
    if (file_mapping == nullptr) {
      auto temp_path = GetTempFileName(mapping_name);
      if (temp_path.empty()) return nullptr;

      auto file_handle = CreateFileA(
          temp_path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
          nullptr, is_resume ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
      if (file_handle == INVALID_HANDLE_VALUE) {
        SHMLOG_ERROR("[shm_win] open file failed, error:%lu", GetLastError());
        return nullptr;
      }
      auto size_high = static_cast<DWORD>(size >> 32);
      auto size_low = static_cast<DWORD>(size & 0xffffffff);
      file_mapping = CreateFileMappingA(file_handle, nullptr, PAGE_READWRITE, size_high, size_low,
                                        mapping_name.c_str());
      CloseHandle(file_handle);
    }
    if (file_mapping == nullptr) {
      SHMLOG_ERROR("[shm_win] create file mapping failed, error:%lu", GetLastError());
      return nullptr;
    }
    return file_mapping;
  }

  static std::string GetTempFileName(std::string& mapping_name) {
    char temp_path[MAX_PATH];
    auto len = GetTempPathA(sizeof(temp_path), temp_path);
    if (len == 0) {
      SHMLOG_ERROR("[shm_win] get temp path failed, error:%lu", GetLastError());
      return "";
    }

    auto left_len = sizeof(temp_path) - len;
    auto ret = snprintf(temp_path + len, left_len, "%s", mapping_name.c_str());
    if (ret < 0 || ret >= (int)left_len) {
      SHMLOG_ERROR("[shm_win] contact temp path failed, error:%lu", GetLastError());
      return "";
    }
    return temp_path;
  }
};
