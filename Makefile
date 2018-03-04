# ------------------------------------------------------------------------------
# Psyence makefile.
#
# Sections:
#   1. Flags and directories
#   2. File collections
#   3. Tests
#   4. Build and clean
#   5. Makefile debug
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# 1. Flags and directories

BIN_ROOT = bin
BUILD_ROOT = build
SRC_ROOT = src

CXX = clang++-5.0

WARN_NO_FLAGS = \
    -Wno-c++98-compat \
    -Wno-c++98-compat-pedantic \
    -Wno-double-promotion \
    -Wno-format-nonliteral \
    -Wno-padded \
    $(BASE_STR_FLAGS) \

WARN_FLAGS = \
    -Wall \
    -Werror \
    -Weverything \
    -Wextra \
    -Wpedantic \
    $(WARN_NO_FLAGS) \

CXX_FLAGS = \
    -Isrc/ \
    -O3 \
    -std=c++17 \
    $(WARN_FLAGS) \

LD_FLAGS = \
	-lboost_system \
	-lboost_thread \
	-lpthread

# ------------------------------------------------------------------------------
# 2. File collections

MAIN_SOURCES = $(shell find $$SRC_ROOT -type f -name "*.cc" | \
    xargs grep "^int main" | cut -d: -f1 | cut -c3- | sort)

LIB_SOURCES = $(shell find $$SRC_ROOT -type f -name "*.cc" | \
    xargs grep -L "^int main" | cut -c3- | sort)

OBJS = $(patsubst $(SRC_ROOT)/%.cc, $(BUILD_ROOT)/%.o, $(LIB_SOURCES))

HEADERS = $(shell find src/ -type f -name "*.h" | sort)

BINS = $(patsubst $(SRC_ROOT)/%.cc, $(BIN_ROOT)/%, $(MAIN_SOURCES))

$(BUILD_ROOT)/%.o: $(SRC_ROOT)/%.cc $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(BIN_ROOT)/%: $(SRC_ROOT)/%.cc $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(OBJS) $< -o $@ $(LD_FLAGS)

.SECONDARY: $(OBJS)

# ------------------------------------------------------------------------------
# 3. Tests

NO_COLOR = "\\033[0m"
OK_COLOR = "\\033[32;01m"
ERROR_COLOR = "\\033[31;01m"
PASS_MSG = $(OK_COLOR)[PASS]$(NO_COLOR)
FAIL_MSG = $(ERROR_COLOR)[FAIL]$(NO_COLOR)

run-$(BIN_ROOT)/%: $(BIN_ROOT)/%
	@./$< || (printf $(FAIL_MSG) && printf " " && printf "$<\n" && false)
	@printf $(PASS_MSG)
	@printf " "
	$<

TEST_BINS = $(patsubst $(SRC_ROOT)/%.cc, $(BIN_ROOT)/%, \
			  $(shell find $(SRC_ROOT) -type f -name "*_test.cc" | sort))

TEST_CMDS = $(patsubst %, run-%, $(TEST_BINS))

test: $(TEST_CMDS)

# ------------------------------------------------------------------------------
# 4. Build and clean

all: $(BINS)

clean:
	@rm -rf $(BIN_ROOT) $(BUILD_ROOT)

# ------------------------------------------------------------------------------
# 5. Makefile debug

print-%:
	@echo $* = $($*)
