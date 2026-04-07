# QtGPT Makefile - Qt6 Widgets + C++17

QT += core gui widgets

# Check for Qt6
QT_PATH := $(shell pkg-config --path-cflags Qt6)
QT_LIBS := $(shell pkg-config --libs Qt6)
QMAKE := $(shell pkg-config --variable=prefix Qt6)/bin/qmake

# C++ standard
CXXFLAGS += -std=c++17 -Wall -Wextra

# Include paths
INCLUDES = -I/usr/include/disasterparty $(QT_PATH)

# Library paths
LIBS = -ldisasterparty $(shell pkg-config --libs Qt6) -lcurl -lcjson

# Project files
TARGET = qtgpt
SOURCES = main.cpp QtGPT.cpp ChatWidget.cpp SettingsDialog.cpp \
          PipeHandler.cpp ThreadWorker.cpp PluginManager.cpp \
          Plugin.cpp Tool.cpp WeatherTool.cpp StockTool.cpp utils.cpp

HEADERS = QtGPT.h ChatWidget.h SettingsDialog.h PipeHandler.h \
          ThreadWorker.h PluginManager.h Plugin.h Tool.h \
          WeatherTool.h StockTool.h utils.h

# Compiler
CXX = g++

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link target
$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

# Compile source files
%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (optional)
install:
	install -d -m 755 $(DESTDIR)/usr/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/bin/

.PHONY: all clean install
