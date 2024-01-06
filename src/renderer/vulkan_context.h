#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

typedef struct {
    VkSwapchainKHR swapchain;
    VkImage *images;
    VkFormat image_format;
    VkExtent2D extent;
} SwapchainContext;

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

#include "device.h"
#include "swapchain.h"

// Creation
VulkanContext *create_vulkan_context(GLFWwindow *window);
SwapchainContext *create_swapchain_context(VulkanContext *v_ctx, GLFWwindow *window);

// Cleanup
void destroy_vulkan_context(VulkanContext *v_ctx);
void destroy_swapchain_context(VkDevice device, SwapchainContext *swapchain_ctx);

#endif