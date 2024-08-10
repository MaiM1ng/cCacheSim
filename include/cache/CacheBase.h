#ifndef __CACHE_BASE_H
#define __CACHE_BASE_H

#include "utils.h"
#include <algorithm>
#include <cmath>
#include <common.h>
#include <cstdint>
#include <vector>

class CacheBase {
public:
  CacheBase(size_t line_size, size_t line_num) {
    if (line_size == 0 || line_num == 0) {
      assert(0);
    }
    this->line_num = line_num;
    this->line_size = line_size;

    uint64_t index_width = log2(this->line_num);
    this->index_start = 2; // 4B
    this->index_end = this->index_start + index_width - 1;

    uint64_t tag_width = 32 - 2 - index_width;
    this->tag_start = this->index_end + 1;
    this->tag_end = this->tag_start + tag_width - 1;

    this->tag.resize(this->line_num);
    this->valid.resize(this->line_num);
    this->cacheData.resize(this->line_num,
                           std::vector<uint32_t>(this->line_size));

    std::fill(this->valid.begin(), this->valid.end(), 0);
  }

  size_t getLineSize() { return this->line_size; }

  size_t getLineNum() { return this->line_num; }

  bool check_in_cache(uint32_t inst) {
    uint32_t tag = BITS(inst, this->tag_end, this->tag_start);
    uint32_t idx = BITS(inst, this->index_end, this->index_start);
    // printf("----\n");
    // printf("tag: %x\n", tag);
    // printf("idx: %x\n", idx);

    if (this->tag[idx] == tag && this->valid[idx]) {
      // printf("hit: data = %x\n", this->cacheData[idx][0]);
      return true;
    }

    // printf("miss\n");
    // printf("----\n");

    return false;
  }

  void update_cache(uint32_t inst) {
    uint32_t tag = BITS(inst, this->tag_end, this->tag_start);
    uint32_t idx = BITS(inst, this->index_end, this->index_start);

    this->valid[idx] = 1;
    this->tag[idx] = tag;
    this->cacheData[idx][0] = inst;
  }

  void show() {
    printf("Cache Config:\n");
    printf("=============\n");

    printf("Cache Line Num : %lu\n", this->line_num);
    printf("Cache Line Size: %lu\n", this->line_size);

    printf("offset range: [ 1: 0]\n");
    printf("index  range: [%02lu:%02lu]\n", this->index_end, this->index_start);
    printf("tag    range: [%02lu:%02lu]\n", this->tag_end, this->tag_start);
    printf("=============\n");
  }

private:
  size_t line_size;
  size_t line_num;

  size_t tag_end;
  size_t tag_start;

  size_t index_end;
  size_t index_start;

  std::vector<std::vector<uint32_t>> cacheData;
  std::vector<uint32_t> tag;
  std::vector<uint8_t> valid;
};

#endif
