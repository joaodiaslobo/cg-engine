cmake_minimum_required(VERSION 3.25)
project(cg-engine)

set(CMAKE_CXX_STANDARD 17)

if (DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

add_subdirectory(generator)
add_subdirectory(engine)