cmake_minimum_required(VERSION 3.0)
project(vrender C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Source files
file(GLOB_RECURSE SOURCE_FILES_DIR "${CMAKE_SOURCE_DIR}/src/*.c")
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADER_BIN_DIR "${CMAKE_BINARY_DIR}/shaders")
# Ensure the shader binary directory exists
if(NOT EXISTS ${SHADER_BIN_DIR})
    file(MAKE_DIRECTORY ${SHADER_BIN_DIR})
endif()

# Get all shader files
file(GLOB_RECURSE SHADER_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

# Pass directorie(s) into config file
configure_file(
    "${CMAKE_SOURCE_DIR}/src/renderer/config.h.in"
    "${CMAKE_SOURCE_DIR}/src/renderer/config.h"
)

# Get packages
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

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

# Set link libraries
list(APPEND LINK_LIBS Vulkan::Vulkan)

# Build options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall") # -Werror

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/vendor/include")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES_DIR})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

# Compile shaders
set(SPIRV_BINARY_FILES "")
foreach(GLSL_SHADER ${SHADER_FILES})
    get_filename_component(FILE_NAME ${GLSL_SHADER} NAME_WE)
    set(SPIRV_OUT "${SHADER_BIN_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV_OUT}
        COMMAND glslc ${GLSL_SHADER} -o ${SPIRV_OUT}
        DEPENDS ${GLSL_SHADER}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUT})
endforeach(GLSL_SHADER)

# Add Shaders target dependency
add_custom_target(shaders ALL DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(${PROJECT_NAME} shaders)

# To build on windows:
# cmake .. -G "Unix Makefiles"