#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

typedef struct {
    VkSwapchainKHR swapchain;
    uint32_t image_count;
    VkImage *images;
    VkExtent2D extent;
    VkFormat image_format;
    VkImageView *image_views;
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

#endif