TARGET_EXEC_SHARED := libdrawing3d.so
TARGET_EXEC_STATIC := libdrawing3d.a

BUILD_DIR := ./build
SRC_DIRS := ./src
INC_DIR := ./include
CFLAGS := -O2 -fPIC
CXXFLAGS := -O2 -fPIC
LDFLAGS_SHARED := -lSDL2 -lcairo --shared -fPIC
LDFLAGS_STATIC := 

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

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

all: $(BUILD_DIR)/$(TARGET_EXEC_SHARED) $(BUILD_DIR)/$(TARGET_EXEC_STATIC)

# The final build step for the shared library
$(BUILD_DIR)/$(TARGET_EXEC_SHARED): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS_SHARED)

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


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)