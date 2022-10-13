EXTENSION_FILE = cpp
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include

RM = del /Q
MKDIR = 
MV = mov
ifeq ($(shell uname), Linux)
	RM = rm -f
	MV = mv
	MKDIR = mkdir -p
endif

ifeq ($(EXTENSION_FILE),cpp)
	COMPILER = g++
else
	COMPILER = gcc
endif
