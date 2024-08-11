#include "instflow/instflow.h"
#include <cache/CacheExec.h>
#include <cache/ReplacementStrategy.h>
#include <csignal>
#include <cstdint>
#include <engine/task.h>
#include <instflow/instflow.h>
#include <thread>
#include <vector>

void single_task(size_t line_size, size_t way_num, size_t block_num,
                 size_t strategy_id) {
  CacheExec cache(line_size, way_num, block_num, strategy_id);

  printf("create: %s\n", cache.file_name.c_str());

  cache.show_cache_config();

  for (cache.pc = 0; cache.pc < inst_flow.nr_inst; cache.pc++) {
    uint32_t inst = inst_flow.inst_records[cache.pc];
    cache.exec_once(inst);
  }

  cache.show_cache_perf();

  fflush(cache.fp);

  printf("task: %s done.\n", cache.file_name.c_str());
}

void task_create_and_run() {
  std::vector<std::thread> threads;

  uint64_t cache_cap = 64;
  printf("Given Capacity: %ldB\n", cache_cap);

  for (int strategy_id = 0; strategy_id < NR_STRATEGY; strategy_id++) {
    // line size
    for (size_t line_size = 4; line_size <= cache_cap; line_size *= 2) {
      for (size_t block_num = 1; block_num <= cache_cap; block_num *= 2) {
        if (line_size * block_num > cache_cap) continue;

        size_t way_num = cache_cap / line_size / block_num;
        threads.emplace_back(single_task, line_size, way_num, block_num, strategy_id);
      }
    }
  }

  for (auto& t: threads) {
    t.join();
  }
}
