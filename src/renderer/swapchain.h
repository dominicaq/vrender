#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan_context.h"
#include "device.h"

// Number of frames we will buffer in the swapchain
#define NO_BUFFERING 1
#define DOUBLE_BUFFERING 2
#define TRIPLE_BUFFERING 3

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;

    uint32_t format_count;
    VkSurfaceFormatKHR *formats;

    uint32_t present_mode_count;
    VkPresentModeKHR *present_modes;
} SwapChainSupportDetails;

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