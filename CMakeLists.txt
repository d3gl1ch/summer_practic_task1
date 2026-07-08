# CMakeLists.txt - корневой файл сборки проекта.
#
# Коликов Глеб Александрович
# Группа МК-101

cmake_minimum_required(VERSION 3.10)
project(file_replacer C)

# Подключение вспомогательных функций
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MyFunctions)

# Размер блока чтения (N) задается при компиляции, по умолчанию 4096
if(NOT DEFINED BLOCK_SIZE)
    set(BLOCK_SIZE 4096)
endif()
add_definitions(-DBLOCK_SIZE=${BLOCK_SIZE})

add_subdirectory(lib)
add_subdirectory(app)