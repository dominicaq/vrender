#include "swapchain.h"

/*
* Swapchain creation
*/
VkSwapchainKHR create_swapchain(VkDevice device, VkSurfaceKHR surface) {
    VkSwapchainCreateInfoKHR create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = NULL;
    create_info.flags = 0;
    create_info.surface = surface;

    // Image data
    create_info.minImageCount = MIN_IMAGE_COUNT;
    // uint32_t                         minImageCount;
    // VkFormat                         imageFormat;
    // VkColorSpaceKHR                  imageColorSpace;
    // VkExtent2D                       imageExtent;
    // uint32_t                         imageArrayLayers;
    // VkImageUsageFlags                imageUsage;
    // VkSharingMode                    imageSharingMode;

    // Queue indices data
    // uint32_t                         queueFamilyIndexCount;
    // const uint32_t*                  pQueueFamilyIndices;

    // Presentation
    // VkSurfaceTransformFlagBitsKHR    preTransform;
    // VkCompositeAlphaFlagBitsKHR      compositeAlpha;
    // VkPresentModeKHR                 presentMode;
    // VkBool32                         clipped;
    // VkSwapchainKHR                   oldSwapchain;

    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(device, &create_info, NULL, &swapchain) != VK_SUCCESS) {
        return NULL;
    }
    return swapchain;
}

/*
* Swapchain support detail
*/
SwapChainSupportDetails *query_swapchain_support_details(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    SwapChainSupportDetails *details = malloc(sizeof(SwapChainSupportDetails));
    if (details == NULL) { return NULL; }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
    details->formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
    if (details->formats == NULL) { return NULL; }

    details->format_count = format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details->formats);

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
    details->present_modes = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
    if (details->present_modes == NULL) { return NULL; }

    details->present_mode_count = present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details->present_modes);

    return details;
}

void destroy_swapchain_support_details(SwapChainSupportDetails *details) {
    free(details->formats);
    free(details->present_modes);
    free(details);
}