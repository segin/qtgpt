cmake_minimum_required(VERSION 3.16)

project(QtGPT VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# Find dependencies using pkg-config and find_library
find_package(PkgConfig REQUIRED)

pkg_check_modules(CJSON REQUIRED libcjson)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(DP REQUIRED disasterparty)

# Enable auto-MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Create executable (Qt6 macro)
qt_add_executable(QtGPT
    src/main.cpp
    src/QtGPT.cpp
    src/ChatWidget.cpp
    src/ChatHistoryViewer.cpp
    src/MenuBar.cpp
    src/PipeHandler.cpp
    src/ThreadWorker.cpp
    src/PluginManager.cpp
    src/Plugin.cpp
    src/Tool.cpp
    src/WeatherTool.cpp
    src/StockTool.cpp
    src/utils.cpp
    src/SettingsDialog.cpp
    src/QtGPT.h
    src/ChatWidget.h
    src/ChatHistoryViewer.h
    src/MenuBar.h
    src/PipeHandler.h
    src/ThreadWorker.h
    src/PluginManager.h
    src/Plugin.h
    src/Tool.h
    src/WeatherTool.h
    src/StockTool.h
    src/utils.h
    src/PipeMessage.h
    src/SettingsDialog.h
)

# Include directories
target_include_directories(QtGPT PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CJSON_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${DP_INCLUDE_DIRS}
)

# Target properties
target_compile_features(QtGPT PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(QtGPT PRIVATE /W4)
else()
    target_compile_options(QtGPT PRIVATE -Wall -Wextra -Wpedantic -fsanitize=address)
    target_link_options(QtGPT PRIVATE -fsanitize=address)
endif()

# Link libraries (order matters)
target_link_libraries(QtGPT PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    ${DP_LIBRARIES}
    ${CJSON_LIBRARIES}
    ${CURL_LIBRARIES}
    dl
    pthread
)

# Install
install(TARGETS QtGPT
    RUNTIME DESTINATION bin
)