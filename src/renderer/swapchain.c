#include "swapchain.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/*
* Swapchain creation
*/
VkSurfaceFormatKHR get_swapchain_surface_format(VkSurfaceFormatKHR *formats, uint32_t format_count) {
    for (int i = 0; i < format_count; ++i) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }

    return formats[0];
}

VkPresentModeKHR get_swapchain_present_mode(VkPresentModeKHR *present_modes, uint32_t present_mode_count) {
    for (int i = 0; i < present_mode_count; ++i) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_modes[i];
        }
    }
    return present_modes[0];
}

VkExtent2D get_swapchain_extent(VkSurfaceCapabilitiesKHR capabilities, GLFWwindow *window) {
    if (capabilities.currentExtent.width != INT_MAX) {
        return capabilities.currentExtent;
    }

    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    width = CLAMP(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    height = CLAMP(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    VkExtent2D actual_extent = {
        (uint32_t)width,
        (uint32_t)height
    };

    return actual_extent;
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

/*
* Image views
*/
VkImageView *create_swapchain_image_views(VkDevice device, SwapchainContext *swapchain_ctx, uint32_t image_count) {
    VkImageView *image_views = malloc(sizeof(VkImageView) * image_count);
    if (image_views == NULL) { return NULL; }

    for (int i = 0; i < image_count; ++i) {
        VkImageViewCreateInfo create_info;
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.pNext = NULL;
        create_info.image = swapchain_ctx->images[i];

        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swapchain_ctx->image_format;

        // Components
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Subresource range
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &create_info, NULL, &image_views[i]) != VK_SUCCESS) {
            fprintf(stderr, "failed to create swapchain image view [%d]\n", i);
            return NULL;
        }
    }

    return image_views;
}

/*
* Cleanup
*/
void destroy_swapchain_support_details(SwapChainSupportDetails *details) {
    free(details->formats);
    free(details->present_modes);
    free(details);
}

void destroy_swapchain_context(VkDevice device, SwapchainContext *swapchain_ctx) {
    for (int i = 0; i < swapchain_ctx->image_count; ++i) {
        vkDestroyImageView(device, swapchain_ctx->image_views[i], NULL);
    }
    // Destroying swapchain destroys VkImages aquired by vkGetSwapchainImagesKHR
    vkDestroySwapchainKHR(device, swapchain_ctx->swapchain, NULL);

    free(swapchain_ctx->image_views);
    free(swapchain_ctx->images);
    free(swapchain_ctx);
}