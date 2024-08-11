#ifndef __REPLACEMENT_STRATEGY_H
#define __REPLACEMENT_STRATEGY_H

#include <cerrno>
#include <common.h>
#include <cstdint>
#include <cstdlib>
#include <time.h>
#include <vector>

enum {
  RANDOM_MAP,
  LRU_MAP,
  FIFO_MAP,

  NR_STRATEGY
};

class ReplacementStrategy {
public:
  // in block
  virtual size_t selectVictim(const std::vector<uint8_t> &valid,
                              const std::vector<uint32_t> &count) = 0;

  virtual const char *getName() = 0;
};

class RandomReplacement : public ReplacementStrategy {
public:
  size_t selectVictim(const std::vector<uint8_t> &valid,
                      const std::vector<uint32_t> &count) override {
    size_t n = valid.size();

    for (size_t i = 0; i < n; i++) {
      if (!valid[i]) { // 有空闲的
        return i;
      }
    }

    // 到这说明全部都满了
    // 任选一个
    srand(time(NULL));

    return rand() % n;
  }

  const char *getName() override { return "Random"; }
};

class LRUReplacement : public ReplacementStrategy {
public:
  size_t selectVictim(const std::vector<uint8_t> &valid,
                      const std::vector<uint32_t> &count) override {
    size_t n = valid.size();
    int64_t maxCnt = -1;
    size_t idx = -1;

    if (n != count.size()) {
      assert(0);
    }

    for (size_t i = 0; i < n; i++) {
      if (!valid[i]) {
        return i;
      } else {
        if (count[i] > maxCnt) {
          maxCnt = count[i];
          idx = i;
        }
      }
    }

    return idx;
  }

  const char *getName() override { return "LRU"; }
};

class FIFOReplacement : public ReplacementStrategy {
public:
  size_t selectVictim(const std::vector<uint8_t> &valid,
                      const std::vector<uint32_t> &count) override {
    size_t n = valid.size();

    for (size_t i = 0; i < n; i++) {
      if (!valid[i]) {
        return i;
      }
    }

    size_t res = curIdx;

    curIdx = (curIdx + 1) % n;

    return res;
  }

  const char *getName() override { return "FIFO"; }

private:
  size_t curIdx = 0;
};

#endif
