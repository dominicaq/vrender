#ifndef DEVICE_H
#define DEVICE_H

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include "validation_layers.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int16_t graphics_index, present_index;
} QueueFamilyIndices;

typedef struct {
    VkInstance instance;
    VkDevice logical_device;
    QueueFamilyIndices *indices;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
} VulkanDevice;

extern const bool ENABLE_VALIDAITON_LAYERS;
extern const uint32_t VALIDATION_LAYER_COUNT;
extern const char *VALIDATION_LAYERS[];

// Device creation
VulkanDevice *create_vulkan_device();
VkInstance create_instance();
VkPhysicalDevice get_physical_device(VkInstance instance);
VkDevice create_logical_device(VkPhysicalDevice physical_device, VkQueueFamilyProperties *family_properties, uint32_t family_count) ;

// Extensions
bool is_physical_device_suitable(VkPhysicalDevice physical_device);
VkExtensionProperties *get_available_extensions(uint32_t *extension_count);
const char **get_required_extensions(uint32_t *extension_count);

// Queue families
VkQueueFamilyProperties *get_queue_family_properties(VkPhysicalDevice physical_device, uint32_t *family_count);
QueueFamilyIndices *get_unique_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VkQueueFamilyProperties *family_properties, uint32_t family_count);
VkDeviceQueueCreateInfo *create_queue_family_create_infos(VkQueueFamilyProperties *family_properties, uint32_t family_count);

// Cleanup
void destroy_vulkan_device(VulkanDevice *device);

// Debugging
void print_extensions();

#endif