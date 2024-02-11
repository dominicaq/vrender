#ifndef PIPELINE_H
#define PIPELINE_H

#include "vulkan_context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

extern const int DYNAMIC_STATES_COUNT;
extern const VkDynamicState DYNAMIC_STATES[];

// Pipeline creation
VkPipelineLayout create_graphics_pipeline(VkDevice device, SwapchainContext *swapchain_ctx, const char *fname_vert, const char *fname_frag);
VkPipelineLayout create_pipeline_layout(VkDevice device, SwapchainContext *swapchain_ctx);

// Render pass
VkRenderPass create_render_pass(VkDevice device);

// Shaders
VkShaderModule create_shader_module(VkDevice device, const char *fname);
uint32_t *read_file(const char *fname, size_t *bytes_read);

#endif