cmake_minimum_required(VERSION 3.0)
project(vrender C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Automatically include all source files
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")

# Platform-specific settings
if(APPLE)
    find_package(glfw3 REQUIRED)
    if(NOT glfw3_FOUND)
        message(FATAL_ERROR "GLFW not found")
    endif()
    set(LINK_LIBS glfw)
elseif(WIN32)
    set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/vendor/lib")
    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/vendor/include")
    include_directories(${GLFW_INCLUDE_DIR})
    find_library(GLFW_LIBRARY NAMES glfw3 glfw PATHS ${GLFW_LIB_DIR} NO_DEFAULT_PATH)
    if(NOT GLFW_LIBRARY)
        message(FATAL_ERROR "GLFW library not found")
    endif()
    set(LINK_LIBS ${GLFW_LIBRARY})
endif()

# Find Vulkan package
find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/vendor/include")

# Build options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall") # -Werror

# Create the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} Vulkan::Vulkan)

# To build on windows:
# cmake .. -G "Unix Makefiles"