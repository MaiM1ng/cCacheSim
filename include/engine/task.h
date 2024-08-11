#ifndef __TASK_H
#define __TASK_H

#include <common.h>

void task_create_and_run();
void single_task(size_t line_size, size_t way_num, size_t block_num,
                 size_t strategy_id);

#endif
