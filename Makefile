# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -fPIC -Iinclude -Ivendor
LDFLAGS = -lcurl

# Target library name
TARGET_LIB = libaxion

# Source files
# We find all .c files in the src and vendor directories
SRCS = $(wildcard src/*.c) $(wildcard vendor/*.c)

# Object files
# We replace the .c extension with .o for all source files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET_LIB).so $(TARGET_LIB).a example

# Shared library
$(TARGET_LIB).so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(LDFLAGS)
	@echo "Shared library created: $@"

# Static library
$(TARGET_LIB).a: $(OBJS)
	ar rcs $@ $(OBJS)
	@echo "Static library created: $@"

# Example executable
example: example.c $(TARGET_LIB).so
	$(CC) $(CFLAGS) -o $@ example.c -L. -laxion $(LDFLAGS)
	@echo "Example executable created: $@"

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f src/*.o vendor/*.o *.o *.so *.a example
	@echo "Cleaned up build artifacts."

.PHONY: all clean
