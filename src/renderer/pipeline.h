#ifndef PIPELINE_H
#define PIPELINE_H

#include "vulkan/vulkan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char *SHADER_BIN_DIR;

extern const int DYNAMIC_STATES_COUNT;
extern const VkDynamicState DYNAMIC_STATES[];

// Pipeline creation
void create_graphics_pipeline(VkDevice device, const char *fname_vert, const char *fname_frag);
void input_assembly();
void rasterizer();

// Shaders
VkShaderModule create_shader_module(VkDevice device, const char *fname);
uint32_t *read_file(const char *fname, size_t *bytes_read);

#endif