#ifndef __CACHE_PERF_H
#define __CACHE_PERF_H

#include <common.h>
#include <cstdint>
#include <cstdio>

class CachePerf {
public:
  CachePerf() {
    this->cnt_hit = 0;
    this->cnt_miss = 0;
  }

  void show_perf() {
    printf("Cache Perf:\n");
    printf("=============\n");
    printf("Miss: %lu\n", this->cnt_miss);
    printf("Hit : %lu\n", this->cnt_hit);
    printf("Hit Rate: %lf\n",
           (double)this->cnt_hit / (this->cnt_hit + this->cnt_miss));
    printf("=============\n");
  }

  void add_miss() { this->cnt_miss += 1; }

  void add_hit() { this->cnt_hit += 1; }

private:
  uint64_t cnt_miss;
  uint64_t cnt_hit;
};

#endif // !__CACHE_PERF_H
