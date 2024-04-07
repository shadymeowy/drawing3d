TARGET_EXEC_STATIC := libdrawing3d.a

BUILD_DIR := ./build
SRC_DIRS := ./src
INC_DIR := ./include
EXAMPLE_DIR := ./examples
CFLAGS := -O2 $(shell pkg-config --cflags cairo) $(shell pkg-config --cflags sdl2) -Wall -Wextra -Werror -std=c11
CXXFLAGS := -O2 $(shell pkg-config --cflags cairo) $(shell pkg-config --cflags sdl2) -Wall -Wextra -Werror -std=c++17
LDFLAGS_STATIC := $(shell pkg-config --libs cairo) $(shell pkg-config --libs sdl2) -lm -Wall -Wextra -Werror
LDFLAGS_EXAMPLE := -ldrawing3d $(LDFLAGS_STATIC)

# check platform for adding -fpic or equivalent
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -fPIC
	CXXFLAGS += -fPIC
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += 
	CXXFLAGS += 
endif

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
EXAMPLE_SRCS := $(shell find $(EXAMPLE_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Get the names of the executables we want to build for the examples
EXAMPLE_EXECS := $(EXAMPLE_SRCS:$(EXAMPLE_DIR)/%.c=%)

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += $(shell find $(INC_DIR) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP

all: $(BUILD_DIR)/$(TARGET_EXEC_STATIC) $(EXAMPLE_EXECS)

# The final build step for the static library
$(BUILD_DIR)/$(TARGET_EXEC_STATIC): $(OBJS)
	ar rcs $@ $(OBJS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Build each example
$(EXAMPLE_EXECS): % : $(EXAMPLE_DIR)/%.c $(BUILD_DIR)/$(TARGET_EXEC_STATIC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(BUILD_DIR)/$@ $< $(LDFLAGS_EXAMPLE) -L$(BUILD_DIR)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)