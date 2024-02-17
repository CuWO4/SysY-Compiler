# Based on https://matansilver.com/2017/08/29/universal-makefile/
# Modified by MaxXing

FB_EXT := .cpp

# Flags
CFLAGS := -Wall -std=c11
CXXFLAGS := -Wall -Wno-register -std=c++17
FFLAGS :=
BFLAGS := -d
LDFLAGS :=

# Debug flags
DEBUG ?= 1
ifeq ($(DEBUG), 0)
CFLAGS += -O2
CXXFLAGS += -O2
else
# CFLAGS += -g -O0
# CXXFLAGS += -g -O0
CFLAGS += -O0
CXXFLAGS += -O0
endif

# Compilers
CC := clang
CXX := clang++
FLEX := flex
BISON := bison

# Directories
TOP_DIR := $(shell pwd)
TARGET_EXEC := compiler
SRC_DIR := $(TOP_DIR)/src
BUILD_DIR ?= $(TOP_DIR)/build
LIB_DIR ?= $(CDE_LIBRARY_PATH)/native
INC_DIR ?= $(CDE_INCLUDE_PATH)
CFLAGS += -I$(INC_DIR)
CXXFLAGS += -I$(INC_DIR)
LDFLAGS += -L$(LIB_DIR) -lkoopa

# Source files & target files
FB_SRCS := $(patsubst $(SRC_DIR)/%.l, $(BUILD_DIR)/%.lex$(FB_EXT), $(shell find $(SRC_DIR) -name "*.l"))
FB_SRCS += $(patsubst $(SRC_DIR)/%.y, $(BUILD_DIR)/%.tab$(FB_EXT), $(shell find $(SRC_DIR) -name "*.y"))
SRCS := $(FB_SRCS) $(shell find $(SRC_DIR) -name "*.c" -or -name "*.cpp" -or -name "*.cc")
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.c.o, $(SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(OBJS))
OBJS := $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.cc.o, $(OBJS))
OBJS := $(patsubst $(BUILD_DIR)/%.c, $(BUILD_DIR)/%.c.o, $(OBJS))
OBJS := $(patsubst $(BUILD_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(OBJS))
OBJS := $(patsubst $(BUILD_DIR)/%.cc, $(BUILD_DIR)/%.cc.o, $(OBJS))

# Header directories & dependencies
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_DIRS += $(INC_DIRS:$(SRC_DIR)%=$(BUILD_DIR)%)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))
DEPS := $(OBJS:.o=.d)
CPPFLAGS = $(INC_FLAGS) -MMD -MP

# Main target
$(BUILD_DIR)/$(TARGET_EXEC): $(FB_SRCS) $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -lpthread -ldl -o $@

# C source
define c_recipe
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
endef
$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c; $(c_recipe)
$(BUILD_DIR)/%.c.o: $(BUILD_DIR)/%.c; $(c_recipe)

# C++ source
define cxx_recipe
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
endef
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp; $(cxx_recipe)
$(BUILD_DIR)/%.cpp.o: $(BUILD_DIR)/%.cpp; $(cxx_recipe)
$(BUILD_DIR)/%.cc.o: $(SRC_DIR)/%.cc; $(cxx_recipe)

# Flex
$(BUILD_DIR)/%.lex$(FB_EXT): $(SRC_DIR)/%.l
	mkdir -p $(dir $@)
	$(FLEX) $(FFLAGS) -o $@ $<

# Bison
$(BUILD_DIR)/%.tab$(FB_EXT): $(SRC_DIR)/%.y
	mkdir -p $(dir $@)
	$(BISON) $(BFLAGS) -o $@ $<

-include $(DEPS)

.PHONY : clean once lldb
.PHONY : test-hello-koopa test-hello-riscv
.PHONY : test-lv1-koopa test-lv1-riscv
.PHONY : test-lv3-koopa test-lv3-riscv
.PHONY : docker

once:
	$(CXX) $(SRCS) $(LDFLAGS) -lpthread -ldl -o $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	-rm -rf $(BUILD_DIR)

lldb : $(BUILD_DIR)/$(TARGET_EXEC)
	lldb $(BUILD_DIR)/$(TARGET_EXEC) -- -test ./test/hello/hello.c -o ./test/hello/hello.koopa

test-hello : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -test ./test/hello/hello.c -o ./test/hello/hello.koopa

test-hello-koopa : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -koopa ./test/hello/hello.c -o ./test/hello/hello.koopa

test-hello-riscv : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -riscv ./test/hello/hello.c -o ./test/hello/hello.S

test-lv1-koopa :
	autotest -koopa -s lv1 /root/compiler

test-lv1-riscv :
	autotest -riscv -s lv1 /root/compiler

test-lv3-koopa :
	autotest -koopa -s lv3 /root/compiler

test-lv3-riscv :
	autotest -riscv -s lv3 /root/compiler

test-lv4-koopa :
	autotest -koopa -s lv4 /root/compiler

test-lv4-riscv :
	autotest -riscv -s lv4 /root/compiler

docker :
	docker run -it --rm -v G:/OneDrive/code/projects/sysy-to-riscv--cpp:/root/compiler  maxxing/compiler-dev bash 