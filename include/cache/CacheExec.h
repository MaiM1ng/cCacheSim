#ifndef __CACHE_EXEC_H
#define __CACHE_EXEC_H

#include <cache/CacheBase.h>
#include <cache/CachePerf.h>
#include <cache/ReplacementStrategy.h>
#include <cmath>
#include <common.h>
#include <cstdint>

class CacheExec {
public:
  CacheExec(size_t line_size, size_t way_num, size_t block_num, int strategy_id)
      : cache(line_size, way_num, block_num, strategy_id) {
    this->pc = 0;
  }

  void show_cache_config() { this->cache.show(); }

  void show_cache_perf() { this->perf.show_perf(); }

  void exec_once(uint32_t inst) {
    this->cache.add_counter();

    if (cache.check_in_cache(inst)) {
      // Hit
      this->perf.add_hit();
    } else {
      // Miss
      cache.update_cache(inst);
      this->perf.add_miss();
    }
  }

public:
  uint64_t pc;

private:
  CacheBase cache;
  CachePerf perf;
};

#endif
