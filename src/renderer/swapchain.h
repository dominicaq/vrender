#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "vulkan/vulkan.h"
#include "device.h"

#define MIN_IMAGE_COUNT 2

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;

    uint32_t format_count;
    VkSurfaceFormatKHR *formats;

    uint32_t present_mode_count;
    VkPresentModeKHR *present_modes;
} SwapChainSupportDetails;

VkSwapchainKHR create_swapchain(VkDevice device, VkSurfaceKHR surface);

// Support detail
SwapChainSupportDetails *query_swapchain_support_details(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
void destroy_swapchain_support_details(SwapChainSupportDetails *details);

#endif