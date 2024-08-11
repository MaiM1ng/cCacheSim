#ifndef __CACHE_EXEC_H
#define __CACHE_EXEC_H

#include <cache/CacheBase.h>
#include <cache/CachePerf.h>
#include <cache/ReplacementStrategy.h>
#include <cmath>
#include <common.h>
#include <cstdint>
#include <cstdio>
#include <string>

class CacheExec {
public:
  CacheExec(size_t line_size, size_t way_num, size_t block_num, int strategy_id)
      : cache(line_size, way_num, block_num, strategy_id) {
    this->pc = 0;

    std::string str_strategy(this->cache.strategy->getName());
    this->file_name = str_strategy + "-" + std::to_string(block_num) + "-" +
                      std::to_string(way_num) + "-" + std::to_string(line_size);
    
    extern char* log_dir;
    std::string str_log_dir(log_dir);
    // printf("log in : %s\n", (str_log_dir + "/" + this->file_name).c_str());
    this->fp = fopen((str_log_dir + "/" + this->file_name).c_str(), "w");
  }

  ~CacheExec() {
    fclose(this->fp);
  }

  void show_cache_config() { this->cache.show(this->fp); }

  void show_cache_perf() { this->perf.show_perf(this->fp); }

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
  std::string file_name;
  FILE *fp;
private:
  CacheBase cache;
  CachePerf perf;
};

#endif
