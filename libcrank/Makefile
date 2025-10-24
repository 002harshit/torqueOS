TARGET=libcrank.a

TOOLCHAIN ?=$(HOME)/opt/cross_i386-elf/bin/i386-elf-
CC=$(TOOLCHAIN)gcc
AR=$(TOOLCHAIN)ar

OBJS = \
math/basic.o  \
math/float.o  \
math/mat4.o \
math/vec2.o \
math/vec3.o \
std/memcmp.o \
std/memcpy.o \
std/memset.o \
std/printf.o \
string/strcat.o \
string/strchr.o \
string/strcmp.o \
string/strcpy.o  \
string/strlen.o \
string/strncat.o \
string/strncmp.o \
string/strncpy.o \
string/strrchr.o \
string/strstr.o \
string/strtok.o \

HEADERS = \
types.h \
std.h \
string.h \
math.h \

CFLAGS =
CFLAGS += -std=gnu99 -ffreestanding -O2 -Wall -Wextra -g
CFLAGS += -I./
CFLAGS +=-masm=intel

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.asm
	yasm -f elf32 $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)
	
.PHONY: clean
