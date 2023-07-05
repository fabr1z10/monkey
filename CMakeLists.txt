cmake_minimum_required(VERSION 3.4...3.18)



IF (WIN32)
	set(CMAKE_PREFIX_PATH "c:/program files (x86)/libpng;c:/program files (x86)/glew")
ENDIF()

#add_definitions(-DNDEBUG)

project(monkey)


set(CMAKE_CXX_STANDARD 17)
set(MONKEY_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PNG REQUIRED)
find_package(yaml-cpp REQUIRED)

add_subdirectory(pybind11)

include_directories(${PYBIND_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${MONKEY_INCLUDE} ${PNG_INCLUDE_DIRS} ${YAML_INCLUDE_DIRS})

file(GLOB MONKEY_SOURCE src/*.cpp src/components/*.cpp src/models/*.cpp src/shapes/*.cpp src/shapes3d/*.cpp src/runners/*.cpp
		src/states/*.cpp src/actions/*.cpp src/shaders/*.cpp src/batch/*.cpp)

pybind11_add_module(monkey ${MONKEY_SOURCE})

target_link_libraries(monkey PUBLIC ${OPENGL_LIBRARIES} ${PNG_LIBRARY} ${YAML_CPP_LIBRARIES} glfw GLEW::GLEW)
target_compile_definitions(monkey
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
