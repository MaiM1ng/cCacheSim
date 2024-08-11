# set output name
CACHESIM := cCacheSim

# set build dif name
BUILD_DIR := build
INCLUDE_DIR := include
SRC_DIR := src
OBJ_DIR := $(BUILD_DIR)/objs

# set compiler
CC := g++
CC_FLAGS := -Wall -g

# set ld
LD := g++
LD_FLAGS := 

# set search dir
INC_PATH := $(abspath $(INCLUDE_DIR))
INC_FLAGS := $(addprefix -I, $(INC_PATH))

CC_FLAGS += $(INC_FLAGS)

# set libs
LIBS := 
LIBS_FLAGS := $(addprefix -l, $(LIBS))

# search src files
## only cpp 
CSRC := $(shell find $(abspath ./$(SRC_DIR)) -name "*.cpp")
OBJS := $(patsubst $(abspath ./$(SRC_DIR)/%), $(abspath ./$(OBJ_DIR)/%), $(CSRC:.cpp=.o))
DEPS := $(OBJS:.o=.d)

# set output flags
# CC_FLAGS += -o $(BUILD_DIR)/$(CACHESIM)

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# set args
IMG_FILE := ./img/itrace
LOG_DIR  := ./build/log
CACHESIM_ARGS += -i $(IMG_FILE)
CACHESIM_ARGS += -l $(LOG_DIR)

default: build

test:
	@echo $(OBJS)

build: $(BUILD_DIR)/$(CACHESIM)

run: $(BUILD_DIR)/$(CACHESIM)
	@./$(BUILD_DIR)/$(CACHESIM) $(CACHESIM_ARGS)

clean:
	-rm -rf ./$(BUILD_DIR)

# gen .o and .d
$(abspath $(OBJ_DIR)/%/%.o): $(SRC_DIR)/%/%.cpp
	@mkdir -p $(dir $@)
	@echo + CC $(notdir  $@)
	$(CC) $(CC_FLAGS) -MMD -c $< -o $@

# Handle files directly under src
$(abspath $(OBJ_DIR)/%.o): $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo + CC $(notdir $@)
	@$(CC) $(CC_FLAGS) -MMD -c $< -o $@

# gen elf-file
$(BUILD_DIR)/$(CACHESIM): $(OBJS)
	@echo + LD $(notdir $@)
	@$(LD) -o $(abspath $@) $(LD_FLAGS) $(OBJS) $(LIBS_FLAGS)

.PHONY: clean build run
