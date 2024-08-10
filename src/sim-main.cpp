#include <common.h>

void init_engine(int argc, char *argv[]);
void engine_loop();

int main(int argc, char *argv[]) {

  init_engine(argc, argv);

  engine_loop();

  return 0;
}
