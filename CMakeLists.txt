cmake_minimum_required(VERSION 3.4...3.18)

project(monkey)

set(CMAKE_CXX_STANDARD 17)

find_package(PNG REQUIRED)

add_subdirectory(pybind11)

include_directories(${PYBIND_INCLUDE_DIR})

file(GLOB MONKEY_SOURCE src/*.cpp)

pybind11_add_module(monkey ${MONKEY_SOURCE})

target_link_libraries(monkey PUBLIC ${PNG_LIBRARY})
target_compile_definitions(monkey
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
