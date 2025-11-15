cmake_minimum_required(VERSION 3.20)
project(cradle VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/core/*.cpp
    ${PROJECT_SOURCE_DIR}/src/overlay/*.cpp
    ${PROJECT_SOURCE_DIR}/src/modules/*.cpp
    ${PROJECT_SOURCE_DIR}/src/util/engine/instance/*.cpp
    ${PROJECT_SOURCE_DIR}/src/util/engine/datamodel/*.cpp
    ${PROJECT_SOURCE_DIR}/src/util/engine/visualengine/*.cpp
    ${PROJECT_SOURCE_DIR}/src/util/engine/wallcheck/*.cpp
    ${PROJECT_SOURCE_DIR}/src/cache/*.cpp
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    spdlog::spdlog_header_only
    imgui::imgui
    fmt::fmt-header-only
    d3d11
    dxgi
    d3dcompiler
    dwmapi
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
