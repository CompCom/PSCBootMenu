TARGET_NAME = boot_menu
CXX = g++
STRIP = strip
CXXFLAGS ?= -O2 -std=c++14 -I.
SDLFLAGS = `sdl2-config --cflags`
LDLIBS = wpa/wpa_ctrl.a -lstdc++fs -lSDL2-2.0 -lpng -lSDL2_mixer -lSDL2_image-2.0 -lSDL2_ttf-2.0 -lpthread
LDFLAGS = 
SOURCES := $(shell find -name "*.cpp")
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET_NAME)
release: $(TARGET_NAME)
	$(CROSS_PREFIX)$(STRIP) $(TARGET_NAME)

$(TARGET_NAME): $(OBJECTS)
	$(CROSS_PREFIX)$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(TARGET_NAME)

%.o: %.cpp
	$(CROSS_PREFIX)$(CXX) $(CXXFLAGS) $(SDLFLAGS) -c $< -o $@

clean:
	rm -f *.o framework/*.o menuitems/*.o wpa/*.o boot_menu

.PHONY: clean release
