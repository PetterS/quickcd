CC      = gcc
CXX     = g++

# Compiler flags with automatic dependency generation
CFLAGS   = -O3 -Wall -Werror -MMD -MP
LDFLAGS  = -lpthread -ldl

# Source Files
SRC_CPP = quickcd-store.cpp
SRC_C   = third-party/sqlite3.c

OBJS = $(SRC_CPP:.cpp=.o) $(SRC_C:.c=.o)

# Default target
all: quickcd-store

# Link the binary
quickcd-store: $(OBJS) Makefile
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# Compile C++ source files
%.o: %.cpp Makefile
	$(CXX) $(CFLAGS) -c $< -o $@

# Compile C source files
%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

# Include auto-generated dependency files
-include $(OBJS:.o=.d)

# Phony targets
.PHONY: all clean format

# Code formatting target
format:
	clang-format -i $(SRC_CPP)
	python3 -m black .

# Clean build artifacts
clean:
	rm -f $(OBJS) quickcd-store $(OBJS:.o=.d)

# ln -s $(pwd)/quickcd-store ~/bin/quickcd-store
# ln -s $(pwd)/quickcd-get.py ~/bin/quickcd-get.py
