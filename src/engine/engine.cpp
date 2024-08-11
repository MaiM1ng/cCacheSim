#include <cache/CacheExec.h>
#include <cache/ReplacementStrategy.h>
#include <common.h>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <getopt.h>
#include <instflow/instflow.h>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>

static char *input_file = NULL;
static char *output_file = NULL;
static char *log_file = NULL;

void init_input_file(char *in) {
  if (in == NULL) {
    assert(0);
  }

  printf("Using: %s\n", in);

  int fd = open(in, O_RDONLY);
  if (fd == -1) {
    perror("Can't open input file");
    exit(1);
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    perror("Can't stat input file");
    close(fd);
    exit(1);
  }

  printf("File Size: %lu\n", sb.st_size);
  char *addr = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) {
    perror("Can't mmap input file");
    close(fd);
    exit(1);
  }

  uint64_t cnt_records = 0;
  inst_flow.inst_records = (uint32_t *)malloc(sizeof(uint32_t) * MAX_NUM_INST);

  // mmap优化
  // 需要修改数据格式
  char *lineStart = addr;
  for (char *p = addr; p < addr + sb.st_size; p++) {
    if (*p == '\n') {
      std::string_view line(lineStart, p - lineStart);
      uint32_t value = std::stoul(std::string(line), nullptr, 16);
      inst_flow.inst_records[cnt_records++] = value;
      lineStart = p + 1;
    }
  }

  inst_flow.nr_inst = cnt_records;

  if (munmap(addr, sb.st_size) == -1) {
    perror("munmap error");
    exit(1);
  }

  printf("records: %lu\n", inst_flow.nr_inst);
  printf("first: %x\n", inst_flow.inst_records[0]);
  printf("last : %x\n", inst_flow.inst_records[inst_flow.nr_inst - 1]);
}

void init_log(char *log) {}

void init_output_file(char *out) {}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
      {"input", required_argument, NULL, 'i'},
      {"output", required_argument, NULL, 'o'},
      {"log", required_argument, NULL, 'l'},
      {0, 0, NULL, 0},
  };

  int o;

  while ((o = getopt_long(argc, argv, "-i:o:l:", table, NULL)) != -1) {
    switch (o) {
    case 'i':
      input_file = optarg;
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'l':
      log_file = optarg;
      break;
    default:
      exit(0);
    }
  }

  return 0;
}

static void welcome() { printf("Welcome to cCacheSim\n"); }

void init_engine(int argc, char *argv[]) {

  parse_args(argc, argv);

  init_instflow();

  init_input_file(input_file);

  init_output_file(output_file);

  init_log(log_file);

  welcome();
}

void engine_loop() {
  CacheExec cache(16, 1, 4, FIFO_MAP);

  cache.show_cache_config();

  for (cache.pc = 0; cache.pc < inst_flow.nr_inst; cache.pc++) {
    uint32_t inst = inst_flow.inst_records[cache.pc];
    cache.exec_once(inst);
  }

  cache.show_cache_perf();
}
