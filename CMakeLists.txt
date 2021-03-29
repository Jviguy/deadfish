cmake_minimum_required(VERSION 3.10)

project(sexfish VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB TARGETS
	"src/*.h"
	"src/*.cpp"
)

add_executable(sexfish ${TARGETS})
