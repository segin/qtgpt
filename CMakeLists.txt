cmake_minimum_required(VERSION 3.16)

project(QtGPT VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Find dependencies
find_package(cjson REQUIRED)
find_package(CURL REQUIRED)
find_package(disasterparty REQUIRED)

# Source files
set(SOURCES
    main.cpp
    QtGPT.cpp
    ChatWidget.cpp
    PipeHandler.cpp
    ThreadWorker.cpp
    PluginManager.cpp
    Plugin.cpp
    Tool.cpp
    WeatherTool.cpp
    StockTool.cpp
    utils.cpp
)

set(HEADERS
    QtGPT.h
    ChatWidget.h
    PipeHandler.h
    ThreadWorker.h
    PluginManager.h
    Plugin.h
    Tool.h
    WeatherTool.h
    StockTool.h
    utils.h
    PipeMessage.h
    SettingsDialog.h
)

# Create executable
qt_add_executable(QtGPT
    ${SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(QtGPT PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${disasterparty_INCLUDE_DIRS}
)

# Target properties
target_compile_features(QtGPT PRIVATE cxx_std_17)

# Link libraries
target_link_libraries(QtGPT PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    disasterparty::disasterparty
    cjson::cjson
    CURL::libcurl
)

# Install
install(TARGETS QtGPT
    RUNTIME DESTINATION bin
)
