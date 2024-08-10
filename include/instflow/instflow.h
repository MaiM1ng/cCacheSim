#ifndef __INSTFLOW_H
#define __INSTFLOW_H

#include <common.h>
#include <cstdint>

#define MAX_NUM_INST 250000000

typedef struct {
  uint32_t *mmap_addr;
  uint32_t *inst_records;
  uint64_t nr_inst;
} InstFlow;

extern InstFlow inst_flow;

void init_instflow();

#endif // ! __INSTFLOW_H
