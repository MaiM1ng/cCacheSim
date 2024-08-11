#ifndef __CACHE_BASE_H
#define __CACHE_BASE_H

#include <algorithm>
#include <cache/ReplacementStrategy.h>
#include <cmath>
#include <common.h>
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <utils.h>
#include <vector>

#define WORD_BITS 32
#define WORD_BYTES (WORD_BITS / 8)

class CacheBase {
public:
  CacheBase(size_t line_size, size_t way_num, size_t block_num,
            size_t strategy_id) {
    if (line_size == 0 || block_num == 0 || way_num == 0) {
      assert(0);
    }
    this->block_num = block_num;
    this->line_size = line_size;
    this->way_num = way_num;
    this->total_cap = this->way_num * this->block_num * this->line_size;

    // cal line offset width
    this->line_offset_width = log2(this->line_size / WORD_BYTES);

    uint64_t index_width = log2(this->block_num);
    this->isHaveIndex = index_width != 0;
    this->index_start = 2 + this->line_offset_width;
    this->index_end = this->index_start + index_width - 1;

    uint64_t tag_width = 32 - 2 - index_width - this->line_offset_width;
    this->tag_start = this->index_end + 1;
    this->tag_end = this->tag_start + tag_width - 1;

    this->tag.resize(this->block_num, std::vector<uint32_t>(this->way_num));
    this->valid.resize(this->block_num, std::vector<uint8_t>(this->way_num));
    this->counter.resize(this->block_num, std::vector<uint32_t>(this->way_num));
    this->usage.resize(this->block_num, std::vector<uint32_t>(this->way_num));
    this->cacheData.resize(this->block_num,
                           std::vector<uint32_t>(this->way_num));

    for (auto &each : this->valid) {
      std::fill(each.begin(), each.end(), 0);
    }

    switch (strategy_id) {
    case FIFO_MAP:
      this->strategy = std::make_shared<FIFOReplacement>();
      break;
    case LRU_MAP:
      this->strategy = std::make_shared<LRUReplacement>();
      break;
    case RANDOM_MAP:
      this->strategy = std::make_shared<RandomReplacement>();
      break;
    default:
      assert(0);
    }
  }

  size_t getLineSize() { return this->line_size; }

  size_t getWayNum() { return this->way_num; }

  size_t getBlockNum() { return this->block_num; }

  bool check_in_cache(uint32_t inst) {
    uint32_t tag = BITS(inst, this->tag_end, this->tag_start);
    uint32_t idx;
    if (this->isHaveIndex) idx = BITS(inst, this->index_end, this->index_start);
    else idx = 0;

    for (size_t i = 0; i < this->way_num; i++) {
      if (this->tag[idx][i] == tag && this->valid[idx][i] == 1) {
        this->counter[idx][i] = 0; // 重置heat
        return true;
      }
    }

    return false;
  }

  void update_cache(uint32_t inst) {
    uint32_t idx;
    if (this->isHaveIndex) idx = BITS(inst, this->index_end, this->index_start);
    else idx = 0;
    
    uint32_t tag = BITS(inst, this->tag_end, this->tag_start);
    size_t in_block_idx =
        this->strategy->selectVictim(this->valid[idx], this->counter[idx]);

    this->valid[idx][in_block_idx] = 1;
    this->tag[idx][in_block_idx] = tag;
    this->cacheData[idx][in_block_idx] = inst;

    this->counter[idx][in_block_idx] = 0;
    this->usage[idx][in_block_idx] = 0;
  }

  void add_counter() {
    for (size_t i = 0; i < this->block_num; i++) {
      for (size_t j = 0; j < this->way_num; j++) {
        if (this->valid[i][j]) {
          this->counter[i][j]++;
        }
      }
    }
  }

  void show(FILE *fp) {
    fprintf(fp, "Cache Config:\n");
    fprintf(fp, "=============\n");
    fprintf(fp, "Cache Block Num : %lu\n", this->block_num);
    fprintf(fp, "Cache Line Size : %lu\n", this->line_size);
    fprintf(fp, "Cache Way Num   : %lu\n", this->way_num);
    fprintf(fp, "Cache Map Way   : %s\n", this->strategy->getName());

    fprintf(fp, "offset range:      [ 1: 0]\n");
    fprintf(fp, "line offset width:     %lu\n", this->line_offset_width);
    if (this->isHaveIndex) {
      fprintf(fp, "index  range:      [%02lu:%02lu]\n", this->index_end, this->index_start);
    } else {
      fprintf(fp, "index  range:      None\n");
    }
    fprintf(fp, "tag    range:      [%02lu:%02lu]\n", this->tag_end, this->tag_start);
    fprintf(fp, "=============\n");
  }

  std::shared_ptr<ReplacementStrategy> strategy;
  bool isHaveIndex;

private:
  size_t line_size;
  size_t block_num;
  size_t way_num;
  size_t total_cap;

  size_t tag_end;
  size_t tag_start;

  size_t index_end;
  size_t index_start;

  size_t line_offset_width;

  std::vector<std::vector<uint32_t>> cacheData;
  std::vector<std::vector<uint32_t>> tag;
  std::vector<std::vector<uint8_t>> valid;
  std::vector<std::vector<uint32_t>> counter; // 有效计数
  std::vector<std::vector<uint32_t>> usage;   // 当前cache line heat
};

#endif
