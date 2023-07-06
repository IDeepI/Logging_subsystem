cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(LoggingSubsystem LANGUAGES CXX)

#OUR CODE
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")

file(GLOB_RECURSE SOURCES	
	"${INCLUDE_DIR}/*.h"
	"${SOURCE_DIR}/*.h"
	"${SOURCE_DIR}/*.cpp"
	"${SOURCE_DIR}/impl/*.h"
	"${SOURCE_DIR}/impl/*.cpp"
)

#Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})
message(STATUS "Executables: ${SOURCES}")

#Add the target includes
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

#Using W versions of WIN API functions
target_compile_definitions(${PROJECT_NAME} PRIVATE -DUNICODE)
