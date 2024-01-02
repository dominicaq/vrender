#ifndef DEVICE_H
#define DEVICE_H

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include "global_config.h"
#include "validation_layers.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    uint32_t graphics_count, transfer_count, compute_count;
    uint32_t *graphics_indicies, *transfer_indicies, *compute_indicies;
} QueueFamilyIndices;

typedef struct {
    VkDevice logical_device;
    QueueFamilyIndices queues_indices;
} Device;

// Device
VkDevice create_logical_device(VkInstance instance);
bool is_physical_device_suitable(VkPhysicalDevice physcial_device);

// Extensions
VkExtensionProperties *get_available_extensions(uint32_t *extension_count);
const char **get_required_extensions(uint32_t *extension_count);
void print_extensions();

// Family queues
void populate_queue_family_indices(VkQueueFamilyProperties *family_properties, uint32_t queue_family_count, QueueFamilyIndices *indices);
VkDeviceQueueCreateInfo *create_queue_family_create_infos(VkPhysicalDevice physical_device, uint32_t queue_family_count, QueueFamilyIndices *indices);
void destroy_family_queue_indicies(QueueFamilyIndices *queue_indices);

#endif