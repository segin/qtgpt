cmake_minimum_required(VERSION 3.16)

project(QtGPT VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Find dependencies using pkg-config and find_library
find_package(PkgConfig REQUIRED)

# Check for required packages
pkg_check_modules(CJSON REQUIRED cjson)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(DISASTERPARTY REQUIRED disasterparty)

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
    ${CJSON_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${DISASTERPARTY_INCLUDE_DIRS}
)

# Target properties
target_compile_features(QtGPT PRIVATE cxx_std_17)

# Link libraries
target_link_libraries(QtGPT PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${CJSON_LIBRARIES}
    ${CURL_LIBRARIES}
    ${DISASTERPARTY_LIBRARIES}
)

# Install
install(TARGETS QtGPT
    RUNTIME DESTINATION bin
)
