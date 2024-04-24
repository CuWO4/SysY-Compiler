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
TOP_DIR := ${CURDIR}
TARGET_EXEC := compiler
SRC_DIR := $(TOP_DIR)/src
BUILD_DIR ?= $(TOP_DIR)/build
LIB_DIR ?= $(CDE_LIBRARY_PATH)/native
INC_DIR ?= $(CDE_INCLUDE_PATH)
LDFLAGS += -L$(LIB_DIR) -lkoopa

# Source files & target files
FB_SRCS := $(patsubst $(SRC_DIR)/%.l, $(BUILD_DIR)/%.lex$(FB_EXT), $(wildcard $(SRC_DIR)/*.l))
FB_SRCS += $(patsubst $(SRC_DIR)/%.y, $(BUILD_DIR)/%.tab$(FB_EXT), $(wildcard $(SRC_DIR)/*.y))
SRCS := $(FB_SRCS) $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(SRCS))
OBJS := $(patsubst $(BUILD_DIR)/%.cpp, $(BUILD_DIR)/%.cpp.o, $(OBJS))

# Header directories & dependencies
INC_DIRS := include/
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
	$(FLEX) -o$@ $(FFLAGS) $<

# Bison
$(BUILD_DIR)/%.tab$(FB_EXT): $(SRC_DIR)/%.y | $(BUILD_DIR)
	$(BISON) $(BFLAGS) -o $@ $<

$(BUILD_DIR) :
	mkdir "$(BUILD_DIR)"

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

run-koopa :
	koopac testcases/hello/hello.koopa | llc --filetype=obj -o build/hello.o
	clang build/hello.o -L$$CDE_LIBRARY_PATH/native -lsysy -o build/hello
	build/hello

run-riscv :
	clang testcases/hello/hello.S -c -o build/hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
	ld.lld build/hello.o -L$$CDE_LIBRARY_PATH/riscv32 -lsysy -o build/hello
	qemu-riscv32-static build/hello

once: $(FB_SRCS) | $(BUILD_DIR)
	$(CXX) $(SRCS) $(LDFLAGS) -lpthread -ldl -o $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	-rm -rf $(BUILD_DIR)

MODE ?= koopa 
lldb: $(BUILD_DIR)/$(TARGET_EXEC)
	lldb $(BUILD_DIR)/$(TARGET_EXEC) -- -$(MODE) ./testcases/hello/hello.c -o ./testcases/hello/hello.koopa

docker :
	docker run -it --rm -v ${CURDIR}:/root/compiler  maxxing/compiler-dev bash 