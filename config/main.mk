define main =
	$(info > Makefile++ 1.4.0)
	$(info > Build type: $(BUILD_TYPE))
	$(info > Target version: $(TARGET_VERSION))
endef

BIN_DIR = bin
SRC_FILES = $(wildcard $(SRC_DIR)/*.$(EXTENSION_FILE)) $(wildcard $(SRC_DIR)/*/*.$(EXTENSION_FILE))
OBJECTS = $(SRC_FILES:%.$(EXTENSION_FILE)=%.o)

INCLUDES = -I$(INCLUDE_DIR)
LIBS = -Llib
BUILD_TYPE = debug
TARGET_VERSION = 0.0.1
BUILD_FLAGS = -Wall -Werror -Wextra $(INCLUDES)

ifeq ($(BUILD_TYPE),release)
	BUILD_FLAGS += -O2 -DNDEBUG
else
	BUILD_FLAGS += -g -O0
endif
