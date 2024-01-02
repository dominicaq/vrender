#ifndef VALIDATION_LAYERS_H
#define VALIDATION_LAYERS_H

#include "vulkan/vulkan.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct {
    uint32_t count;
    char **layers;
    VkDebugUtilsMessengerEXT debug_messenger;
} typedef ValidationConfig;

void create_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT *debug_messenger);

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info);

bool check_validation_layer_support(ValidationConfig *validation_config);

VkResult create_debug_utils_msg_ext(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger);

void destroy_debug_utils_msg_ext(VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator);

#endif