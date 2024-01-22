cmake_minimum_required(VERSION 3.0)
project(vrender C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Source files
file(GLOB_RECURSE SOURCE_FILES_DIR "${CMAKE_SOURCE_DIR}/src/*.c")
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/shaders")
# Ensure the shader binary directory exists
if(NOT EXISTS ${SHADER_BINARY_DIR})
    file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
endif()

# Get all shader files
file(GLOB_RECURSE SHADER_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

# Get packages
find_package(glfw3 REQUIRED)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found")
endif()

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

# Set link libraries
set(LINK_LIBS glfw Vulkan::Vulkan)

# Build options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall") # -Werror

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/vendor/include")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES_DIR})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} Vulkan::Vulkan)

# Compile shaders
set(SPIRV_BINARY_FILES "")
foreach(GLSL_SHADER ${SHADER_FILES})
    get_filename_component(FILE_NAME ${GLSL_SHADER} NAME_WE)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND glslc ${GLSL_SHADER} -o ${SPIRV}
        DEPENDS ${GLSL_SHADER}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL_SHADER)

# Add Shaders target dependency
add_custom_target(shaders ALL DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(${PROJECT_NAME} shaders)

# To build on windows:
# cmake .. -G "Unix Makefiles"