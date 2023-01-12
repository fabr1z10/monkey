cmake_minimum_required(VERSION 3.3)

project(monkey3 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(PNG REQUIRED)

add_subdirectory(pybind11)

include_directories(${PYBIND_INCLUDE_DIR})

file(GLOB MONKEY_SOURCE *.cpp)

pybind11_add_module(monkey3 ${MONKEY_SOURCE})

target_link_libraries(monkey3 PUBLIC ${PNG_LIBRARY})

#add_custom_command(
#        TARGET monkey POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${CMAKE_SOURCE_DIR}/config.out
#        ${CMAKE_CURRENT_BINARY_DIR}/config.out)

#file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config.out2 CONTENT "pippo $<TARGET_FILE_NAME:monkey>")
#file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py INPUT ${CMAKE_SOURCE_DIR}/config.out)
configure_file(
        ${PROJECT_SOURCE_DIR}/setup.py.in
        ${PROJECT_BINARY_DIR}/setup.py.in
        @ONLY)
message(${PROJECT_BINARY_DIR})
file(GENERATE OUTPUT ${PROJECT_BINARY_DIR}/setup.py INPUT ${PROJECT_BINARY_DIR}/setup.py.in)

#message($<TARGET_FILE_NAME:monkey>)
