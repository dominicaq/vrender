#ifndef VALIDATION_LAYERS_H
#define VALIDATION_LAYERS_H

#include "vulkan/vulkan.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

VkDebugUtilsMessengerEXT create_debug_messenger(VkInstance instance);

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info);

bool check_validation_layer_support(uint32_t num_validation_layers, const char **validation_layers);

VkResult create_debug_utils_msg_ext(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger);

void destroy_debug_utils_msg_ext(VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator);

void print_available_layers();

#endif