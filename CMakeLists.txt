cmake_minimum_required(VERSION 3.10.0)
project(LuaExtendLib VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SRC_FILES src/*.c)

add_library(LuaExtendLib SHARED ${SRC_FILES})

target_link_libraries(LuaExtendLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/LuaSrcLib/lua54.lib
)
target_include_directories(LuaExtendLib
    PUBLIC
        src
)