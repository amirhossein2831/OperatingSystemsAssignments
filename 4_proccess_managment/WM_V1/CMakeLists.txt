cmake_minimum_required(VERSION 3.28)
project(WM_V1 C)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})

add_executable(VM_V1 main.c)

target_link_libraries(WM_V1 ${GLIB_LIBRARIES})
