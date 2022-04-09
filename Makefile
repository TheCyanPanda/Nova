CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -pthread -std=c++17 #-Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := NovaServer
INCLUDE  := -Iinclude/
SRC      :=                      \
   $(wildcard src/Network/*.cpp) \
   $(wildcard src/Video/*.cpp) \
   $(wildcard src/*.cpp)         \

# OpenCV - If not found, then check for "opencv.pc" in /usr/local/lib/pkgconfig.
# ( see https://stackoverflow.com/questions/15320267/package-opencv-was-not-found-in-the-pkg-config-search-path)
# Then add to .bashrc:
# PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
# export PKG_CONFIG_PATH
CXXFLAGS +=`pkg-config --cflags opencv`
LDFLAGS +=`pkg-config --libs opencv`

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"
