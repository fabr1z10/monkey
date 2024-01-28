cmake_minimum_required(VERSION 3.4...3.18)



IF (WIN32)
	set(GLEW_USE_STATIC_LIBS true)
	add_definitions(-DYAML_CPP_STATIC_DEFINE -DGLEW_STATIC)
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
find_package (Python COMPONENTS Interpreter Development)
add_subdirectory(pybind11)

include_directories(${PYBIND_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${GLEW_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIR} ${PNG_INCLUDE_DIRS})

link_directories(${YAML_CPP_LIBRARY_DIR})

file(GLOB MONKEY_SOURCE src/*.cpp src/shaders/*.cpp src/batch/*.cpp src/models/*.cpp src/components/*.cpp src/components/controllers/*.cpp
		src/runners/*.cpp src/shapes/*.cpp src/math/*.cpp src/nodes/*.cpp)

pybind11_add_module(monkey ${MONKEY_SOURCE})

target_link_libraries(monkey PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw ${PNG_LIBRARIES} ${YAML_CPP_LIBRARIES})

target_compile_definitions(monkey
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

install(TARGETS monkey COMPONENT python LIBRARY DESTINATION "${Python_SITEARCH}")
