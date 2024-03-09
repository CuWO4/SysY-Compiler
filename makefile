# Based on https://matansilver.com/2017/08/29/universal-makefile/
# Modified by MaxXing CuWO4

MAKEFLAGS += -j20

FB_EXT := .cpp

# Flags
CXXFLAGS := -Wall -Wno-register -std=c++17
FFLAGS :=
BFLAGS := -d
LDFLAGS :=

# Debug flags
DEBUG ?= 1
ifeq ($(DEBUG), 0)
CXXFLAGS += -O2
else
CFLAGS += -g -O0
CXXFLAGS += -g -O0
endif

# Compilers
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
LDFLAGS += -L$(LIB_DIR) -lkoopa

# Source files & target files
FB_SRCS := $(patsubst $(SRC_DIR)/%.l, $(BUILD_DIR)/%.lex$(FB_EXT), $(shell find $(SRC_DIR) -name "*.l"))
FB_SRCS += $(patsubst $(SRC_DIR)/%.y, $(BUILD_DIR)/%.tab$(FB_EXT), $(shell find $(SRC_DIR) -name "*.y"))
SRCS := $(FB_SRCS) $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(SRCS))
OBJS := $(patsubst $(BUILD_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(OBJS))

# Header directories & dependencies
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_DIRS += $(INC_DIRS:$(SRC_DIR)%=$(BUILD_DIR)%)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))
CPPFLAGS = $(INC_FLAGS) -MMD -MP

# Main target
$(BUILD_DIR)/$(TARGET_EXEC): $(FB_SRCS) $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -lpthread -ldl -o $@

# C++ source
define cxx_recipe
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
endef
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR); $(cxx_recipe)
$(BUILD_DIR)/%.cpp.o: $(BUILD_DIR)/%.cpp | $(BUILD_DIR); $(cxx_recipe)

# Flex
$(BUILD_DIR)/%.lex$(FB_EXT): $(SRC_DIR)/%.l | $(BUILD_DIR)
	$(FLEX) $(FFLAGS) -o $@ $<

# Bison
$(BUILD_DIR)/%.tab$(FB_EXT): $(SRC_DIR)/%.y | $(BUILD_DIR)
	$(BISON) $(BFLAGS) -o $@ $<

$(BUILD_DIR) :
	mkdir $(BUILD_DIR)

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

run-koopa :
	koopac test/hello/hello.koopa | llc --filetype=obj -o build/hello.o
	clang build/hello.o -L$$CDE_LIBRARY_PATH/native -lsysy -o build/hello
	build/hello

run-riscv :
	clang test/hello/hello.S -c -o build/hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
	ld.lld build/hello.o -L$$CDE_LIBRARY_PATH/riscv32 -lsysy -o build/hello
	qemu-riscv32-static build/hello

once:
	$(CXX) $(SRCS) $(LDFLAGS) -lpthread -ldl -o $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	-rm -rf $(BUILD_DIR)

lldb : $(BUILD_DIR)/$(TARGET_EXEC)
	lldb $(BUILD_DIR)/$(TARGET_EXEC) -- -koopa ./test/hello/hello.c -o ./test/hello/hello.koopa

test-hello-test : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -test ./test/hello/hello.c

test-hello-koopa : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -koopa ./test/hello/hello.c -o ./test/hello/hello.koopa

test-hello-riscv : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -riscv ./test/hello/hello.c -o ./test/hello/hello.S

test-hello : test-hello-koopa test-hello-riscv

test-hello-koopa-debug : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -koopa ./test/hello/hello.c -o ./test/hello/hello.koopa -dbg-k

test-hello-riscv-debug : $(BUILD_DIR)/$(TARGET_EXEC)
	$(BUILD_DIR)/$(TARGET_EXEC) -riscv ./test/hello/hello.c -o ./test/hello/hello.S -dbg-r

test-hello-debug : test-hello-koopa-debug test-hello-riscv-debug

test-all-koopa :
	autotest -koopa /root/compiler

test-all-riscv :
	autotest -riscv /root/compiler

test-all : test-all-koopa test-all-riscv

test-lv1-koopa :
	autotest -koopa -s lv1 /root/compiler

test-lv1-riscv :
	autotest -riscv -s lv1 /root/compiler

test-lv1 : test-lv1-koopa test-lv1-riscv

test-lv3-koopa :
	autotest -koopa -s lv3 /root/compiler

test-lv3-riscv :
	autotest -riscv -s lv3 /root/compiler

test-lv3 : test-lv3-koopa test-lv3-riscv

test-lv4-koopa :
	autotest -koopa -s lv4 /root/compiler

test-lv4-riscv :
	autotest -riscv -s lv4 /root/compiler

test-lv4 : test-lv4-koopa test-lv4-riscv

test-lv5-koopa :
	autotest -koopa -s lv5 /root/compiler

test-lv5-riscv :
	autotest -riscv -s lv5 /root/compiler

test-lv5 : test-lv5-koopa test-lv5-riscv

test-lv6-koopa :
	autotest -koopa -s lv6 /root/compiler

test-lv6-riscv :
	autotest -riscv -s lv6 /root/compiler

test-lv6 : test-lv6-koopa test-lv6-riscv

test-lv7-koopa :
	autotest -koopa -s lv7 /root/compiler

test-lv7-riscv :
	autotest -riscv -s lv7 /root/compiler

test-lv7 : test-lv7-koopa test-lv7-riscv

test-lv8-koopa :
	autotest -koopa -s lv8 /root/compiler

test-lv8-riscv :
	autotest -riscv -s lv8 /root/compiler

test-lv8 : test-lv8-koopa test-lv8-riscv

test-lv9-koopa :
	autotest -koopa -s lv9 /root/compiler

test-lv9-riscv :
	autotest -riscv -s lv9 /root/compiler

test-lv9 : test-lv9-koopa test-lv9-riscv

test-perf-koopa :
	autotest -koopa -s perf /root/compiler

test-perf-riscv :
	autotest -riscv -s perf /root/compiler

test-perf : test-perf-koopa test-perf-riscv

docker :
	docker run -it --rm -v ${CURDIR}:/root/compiler  maxxing/compiler-dev bash 