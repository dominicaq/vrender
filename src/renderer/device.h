#ifndef DEVICE_H
#define DEVICE_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "validation_layers.h"
#include "swapchain.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Swapchain
typedef struct {
    VkSwapchainKHR swapchain;
    VkImage *images;
    VkFormat image_format;
    VkExtent2D extent;
} SwapchainContext;

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;

    uint32_t format_count;
    VkSurfaceFormatKHR *formats;

    uint32_t present_mode_count;
    VkPresentModeKHR *present_modes;
} SwapChainSupportDetails;

// Vulkan instance
typedef struct {
    int16_t graphics_index, present_index;
} QueueFamilyIndices;

typedef struct {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;
    QueueFamilyIndices *indices;
    VkPhysicalDevice physical_device;
    VkDebugUtilsMessengerEXT debug_messenger;
    SwapchainContext *swapchain_ctx;
} VulkanContext;

// Layers
extern const bool ENABLE_VALIDAITON_LAYERS;
extern const char *VALIDATION_LAYERS[];

// Extensions
extern const char *DEVICE_EXTENSIONS[];

// Context creation
VulkanContext *create_vulkan_context(GLFWwindow *window);
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

// Swapchain
#define NO_BUFFERING 1
#define DOUBLE_BUFFERING 2
#define TRIPLE_BUFFERING 3

// Swapchain creation
SwapchainContext *create_swapchain_context(VulkanContext *v_ctx, GLFWwindow *window);
VkSurfaceFormatKHR get_swapchain_surface_format(VkSurfaceFormatKHR *formats, uint32_t format_count);
VkPresentModeKHR get_swapchain_present_mode(VkPresentModeKHR *present_modes, uint32_t present_mode_count);
VkExtent2D get_swapchain_extent(VkSurfaceCapabilitiesKHR capabilities, GLFWwindow *window);

// Support detail
SwapChainSupportDetails *query_swapchain_support_details(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

// Cleanup
void destroy_swapchain_support_details(SwapChainSupportDetails *details);
void destroy_swapchain_context(VkDevice device, SwapchainContext *swapchain_ctx);

#endif