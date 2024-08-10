#ifndef __CACHE_EXEC_H
#define __CACHE_EXEC_H

#include <algorithm>
#include <cache/CacheBase.h>
#include <cache/CacheExec.h>
#include <cache/CachePerf.h>
#include <cmath>
#include <common.h>
#include <cstdint>
#include <vector>

class CacheExec {
public:
  CacheExec(size_t line_size, size_t line_num) : cache(line_size, line_num) {
    this->pc = 0;
  }

  void show_cache_config() { this->cache.show(); }

  void show_cache_perf() { this->perf.show_perf(); }

  void exec_once(uint32_t inst) {
    if (cache.check_in_cache(inst)) {
      // Hit
      this->perf.add_hit();
    } else {
      // Miss
      cache.update_cache(inst);
      this->perf.add_miss();
    }

    this->pc += 1;
  }

private:
  CacheBase cache;
  CachePerf perf;
  uint64_t pc;
};

#endif
