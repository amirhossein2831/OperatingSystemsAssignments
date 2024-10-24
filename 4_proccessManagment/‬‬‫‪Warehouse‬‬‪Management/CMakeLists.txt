cmake_minimum_required(VERSION 3.28)
project(WarehouseManagement C)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})

add_executable(WarehouseManagement main.c)

target_link_libraries(WarehouseManagement ${GLIB_LIBRARIES})
