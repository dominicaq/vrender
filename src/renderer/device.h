#ifndef DEVICE_H
#define DEVICE_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "vulkan_context.h"
#include "validation_layers.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Layers
extern const bool ENABLE_VALIDATION_LAYERS;
extern const char *VALIDATION_LAYERS[];

// Extensions
extern const char *DEVICE_EXTENSIONS[];

// Context creation
VkInstance create_instance();
VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow *window);

// Device
VkPhysicalDevice get_physical_device(VkInstance instance, VkSurfaceKHR surface);
bool is_physical_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkDevice create_logical_device(VkPhysicalDevice physical_device, VkQueueFamilyProperties *family_properties, uint32_t family_count);

// Instance extension helpers
VkExtensionProperties *get_available_instance_extensions(uint32_t *extension_count);
const char **get_required_instance_extensions(uint32_t *extension_count);

// Queue families
VkQueueFamilyProperties *get_queue_family_properties(VkPhysicalDevice physical_device, uint32_t *family_count);
QueueFamilyIndices *get_unique_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VkQueueFamilyProperties *family_properties, uint32_t family_count);
VkDeviceQueueCreateInfo *create_queue_family_create_infos(VkQueueFamilyProperties *family_properties, uint32_t family_count);

// Cleanup
void destroy_vulkan_context(VulkanContext *device);

// Debugging
void print_instance_extensions();
void print_physical_device_extensions(VkPhysicalDevice physical_device);

#endif