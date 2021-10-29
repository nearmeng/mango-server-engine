#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <atomic>
#include <cstdint>

#include "thread_common.h"

SMT_NS_BEGIN

typedef unsigned char BYTE;

typedef struct kfifo {
  BYTE* buffer;  /* the buffer holding the data */
  uint32_t size; /* the size of the allocated buffer */
  uint32_t in;   /* data is added at offset (in % size) */
  uint32_t out;  /* data is extracted from off. (out % size) */
} kfifo_t;

class KFIFO {
 public:
  // 移植自 Linux 内核的无锁队列实现，注意该实现只适用于单生产者单消费者的通信
  template <class T>
  static T roundup_pow_of_two(T n) {
    return (1U << ((
                       (n) & (1U << 31) ? 31 : (n) & (1U << 30) ? 30
                                           : (n) & (1U << 29)   ? 29
                                           : (n) & (1U << 28)   ? 28
                                           : (n) & (1U << 27)   ? 27
                                           : (n) & (1U << 26)   ? 26
                                           : (n) & (1U << 25)   ? 25
                                           : (n) & (1U << 24)   ? 24
                                           : (n) & (1U << 23)   ? 23
                                           : (n) & (1U << 22)   ? 22
                                           : (n) & (1U << 21)   ? 21
                                           : (n) & (1U << 20)   ? 20
                                           : (n) & (1U << 19)   ? 19
                                           : (n) & (1U << 18)   ? 18
                                           : (n) & (1U << 17)   ? 17
                                           : (n) & (1U << 16)   ? 16
                                           : (n) & (1U << 15)   ? 15
                                           : (n) & (1U << 14)   ? 14
                                           : (n) & (1U << 13)   ? 13
                                           : (n) & (1U << 12)   ? 12
                                           : (n) & (1U << 11)   ? 11
                                           : (n) & (1U << 10)   ? 10
                                           : (n) & (1U << 9)    ? 9
                                           : (n) & (1U << 8)    ? 8
                                           : (n) & (1U << 7)    ? 7
                                           : (n) & (1U << 6)    ? 6
                                           : (n) & (1U << 5)    ? 5
                                           : (n) & (1U << 4)    ? 4
                                           : (n) & (1U << 3)    ? 3
                                           : (n) & (1U << 2)    ? 2
                                           : (n) & (1U << 1)    ? 1
                                           : (n) & (1U << 0)    ? 0
                                                                : -1) +
                   1));
  }
  static void kfifo_init(struct kfifo* fifo, BYTE* buffer, uint32_t size) {
    fifo->buffer = buffer;
    fifo->size = size;

    fifo->in = fifo->out = 0;
  }

  static bool kfifo_alloc(struct kfifo* fifo, uint32_t size) {
    BYTE* buffer;
    if (size & (size - 1)) {
      size = roundup_pow_of_two<uint32_t>(size);
    }
    buffer = (BYTE*)malloc(size);
    if (!buffer) {
      kfifo_init(fifo, NULL, 0);
      return false;
    }

    kfifo_init(fifo, buffer, size);

    return true;
  }

  static void kfifo_free(struct kfifo* fifo) {
    if (fifo->buffer) {
      free(fifo->buffer);
    }
    fifo->buffer = NULL;
    fifo->size = 0;
    fifo->in = fifo->out = 0;
  }

  static uint32_t kfifo_in(struct kfifo* fifo, const void* from, uint32_t len) {
    uint32_t off;
    uint32_t l;

    if (fifo->size - (fifo->in - fifo->out) < len)
      return 0;
    len = std::min(fifo->size - (fifo->in - fifo->out), len);
    std::atomic_thread_fence(std::memory_order_acquire);

    off = fifo->in & (fifo->size - 1);
    l = std::min(len, fifo->size - off);
    memcpy(fifo->buffer + off, from, l);
    memcpy(fifo->buffer, (char*)from + l, len - l);

    std::atomic_thread_fence(std::memory_order_release);
    fifo->in += len;

    return len;
  }

  static uint32_t kfifo_out(struct kfifo* fifo, void* to, uint32_t len) {
    uint32_t off;
    uint32_t l;

    len = std::min(fifo->in - fifo->out, len);
    if (len == 0)
      return 0;
    std::atomic_thread_fence(std::memory_order_acquire);

    off = fifo->out & (fifo->size - 1);
    l = std::min(len, fifo->size - off);
    memcpy(to, fifo->buffer + off, l);
    memcpy((char*)to + l, fifo->buffer, len - l);

    std::atomic_thread_fence(std::memory_order_release);
    fifo->out += len;

    return len;
  }

  static uint32_t kfifo_len(struct kfifo* fifo) {
    return (fifo->in - fifo->out) / sizeof(void*);
  }
};
SMT_NS_END