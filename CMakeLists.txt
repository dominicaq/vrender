cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Project
set(PROJECT_NAME "vrender")
# Automatically include all source files
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")
project(${PROJECT_NAME} C)

# Find Vulkan package
find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

# Vendor
include_directories("${CMAKE_SOURCE_DIR}/vendor/include")
link_directories("${CMAKE_SOURCE_DIR}/vendor/lib")

# Build options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall") # -Werror

# Create the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} glfw3 Vulkan::Vulkan)

# To build on windows:
# cmake .. -G "Unix Makefiles"