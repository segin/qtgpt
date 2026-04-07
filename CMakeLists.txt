cmake_minimum_required(VERSION 3.16)

project(QtGPT VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Find dependencies using pkg-config and find_library
find_package(PkgConfig REQUIRED)

# Check for required packages using find_library
find_library(CJSON_LIBRARY cjson)
find_library(CURL_LIBRARY CURL)
find_library(DISASTERPARTY_LIBRARY disasterparty)

# Create executable (Qt6 macro)
qt_add_executable(QtGPT
    src/main.cpp
    src/QtGPT.cpp
    src/ChatWidget.cpp
    src/PipeHandler.cpp
    src/ThreadWorker.cpp
    src/PluginManager.cpp
    src/Plugin.cpp
    src/Tool.cpp
    src/WeatherTool.cpp
    src/StockTool.cpp
    src/utils.cpp
    src/QtGPT.h
    src/ChatWidget.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/include
    /usr/include/cjson
    /usr/include/curl
    /usr/include/disasterparty
)

# Target properties
target_compile_features(QtGPT PRIVATE cxx_std_17)

# Link libraries (conditionally)
target_link_libraries(QtGPT PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(CJSON_LIBRARY)
    target_link_libraries(QtGPT PRIVATE ${CJSON_LIBRARY})
endif()

if(CURL_LIBRARY)
    target_link_libraries(QtGPT PRIVATE ${CURL_LIBRARY})
endif()

if(DISASTERPARTY_LIBRARY)
    target_link_libraries(QtGPT PRIVATE ${DISASTERPARTY_LIBRARY})
endif()

# Link pthread and dl
find_library(PTHREAD_LIBRARIES pthread)
find_library(DL_LIBRARIES dl)

if(PTHREAD_LIBRARIES)
    target_link_libraries(QtGPT PRIVATE ${PTHREAD_LIBRARIES})
endif()

if(DL_LIBRARIES)
    target_link_libraries(QtGPT PRIVATE ${DL_LIBRARIES})
endif()

# Install
install(TARGETS QtGPT
    RUNTIME DESTINATION bin
)
