include config/utils.mk
include config/main.mk

INCLUDES += `/opt/SDL/bin/sdl2-config --cflags`
LIBS += `/opt/SDL/bin/sdl2-config --libs` -lSDL2_image
BUILD_FLAGS += 
TARGET = App

	
all: $(call main) $(OBJECTS) $(TARGET) 

$(TARGET): $(OBJECTS)
	@$(COMPILER) -o $@ $^ $(LIBS)
	$(info [+] Linking executable...)

%.o: %.$(EXTENSION_FILE)
	@$(COMPILER) -c -o $@ $< $(BUILD_FLAGS)
	$(info [$(COMPILER)] $< ==> $@...)
	
clean:
	$(info Cleaning...)
	@$(RM) $(OBJECTS) $(TARGET)
	$(info Done!)

