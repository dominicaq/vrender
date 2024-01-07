#include "vulkan_context.h"

VulkanContext *create_vulkan_context(GLFWwindow *window) {
    VulkanContext *v_ctx = malloc(sizeof(VulkanContext));
    if (v_ctx == NULL) {
        fprintf(stderr, "failed to alloc VulkanContext\n");
        return NULL;
    }

    // Vulkan instance
    v_ctx->instance = create_instance();
    if (v_ctx->instance == NULL) {
        fprintf(stderr, "failed to create VkInstance\n");
        return NULL;
    }

    // Window surface
    v_ctx->surface = create_surface(v_ctx->instance, window);
    if (v_ctx->surface == NULL) {
        fprintf(stderr, "failed to create VkSurface\n");
        return NULL;
    }

    // Additional validation layers
    if (ENABLE_VALIDAITON_LAYERS) {
        v_ctx->debug_messenger = create_debug_messenger(v_ctx->instance);
        if (v_ctx->debug_messenger == VK_NULL_HANDLE) {
            fprintf(stderr, "failed to create debug messenger\n");
            return NULL;
        }

        printf("[SUCCESS] Validation layers enabled\n");
    }

    // Pick physical device
    v_ctx->physical_device = get_physical_device(v_ctx->instance, v_ctx->surface);
    if (v_ctx->physical_device == NULL) {
        fprintf(stderr, "failed to find physical device\n");
        return NULL;
    }

    // Queue families
    uint32_t family_count = 0;
    VkQueueFamilyProperties *family_properties = get_queue_family_properties(v_ctx->physical_device, &family_count);
    if (family_properties == NULL || family_count == 0) {
        fprintf(stderr, "failed to get queue family properties\n");
        return NULL;
    }

    v_ctx->indices = get_unique_queue_family_indices(v_ctx->physical_device, v_ctx->surface, family_properties, family_count);
    if (v_ctx->indices == NULL) {
        fprintf(stderr, "failed to get family indices\n");
        return NULL;
    }

    // Device creation
    v_ctx->device = create_logical_device(v_ctx->physical_device, family_properties, family_count);
    if (v_ctx->device == NULL) {
        fprintf(stderr, "failed to create logical device\n");
        return NULL;
    }

    // Swapchain
    v_ctx->swapchain_ctx = create_swapchain_context(v_ctx, window);
    if (v_ctx->swapchain_ctx == NULL) {
        fprintf(stderr, "failed to create swapchain\n");
        return NULL;
    }

    free(family_properties);
    return v_ctx;
}

SwapchainContext *create_swapchain_context(VulkanContext *v_ctx, GLFWwindow *window) {
    SwapchainContext *swapchain_ctx = malloc(sizeof(SwapchainContext));
    if (swapchain_ctx == NULL) { return NULL; }

    SwapChainSupportDetails *details = query_swapchain_support_details(v_ctx->physical_device, v_ctx->surface);
    if (details == NULL) { return NULL; }

    uint32_t image_count = details->capabilities.minImageCount + NO_BUFFERING;
    if (details->capabilities.maxImageCount > 0 && image_count > details->capabilities.maxImageCount) {
        image_count = details->capabilities.maxImageCount;
    }

    VkSurfaceFormatKHR surface_format = get_swapchain_surface_format(details->formats, details->format_count);
    VkPresentModeKHR present_mode = get_swapchain_present_mode(details->present_modes, details->present_mode_count);
    VkExtent2D extent = get_swapchain_extent(details->capabilities, window);

    VkSwapchainCreateInfoKHR create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = NULL;
    create_info.flags = 0;
    create_info.surface = v_ctx->surface;

    // Swapchain image
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Queue indices data
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    uint32_t graphics_family = v_ctx->indices->graphics_index;
    uint32_t present_family = v_ctx->indices->present_index;
    uint32_t queueFamilyIndices[2] = {graphics_family, present_family};
    if (graphics_family != present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Presentation
    create_info.preTransform = details->capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    // Swapchain
    swapchain_ctx->image_count = image_count;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(v_ctx->device, &create_info, NULL, &swapchain_ctx->swapchain) != VK_SUCCESS) {
        return NULL;
    }
    // No longer need details
    destroy_swapchain_support_details(details);

    swapchain_ctx->images = malloc(sizeof(VkImage) * image_count);
    if (swapchain_ctx == NULL) { return NULL; }

    vkGetSwapchainImagesKHR(v_ctx->device, swapchain_ctx->swapchain, &image_count, swapchain_ctx->images);
    swapchain_ctx->image_format = surface_format.format;
    swapchain_ctx->extent = extent;
    swapchain_ctx->image_views = create_swapchain_image_views(v_ctx->device, swapchain_ctx, image_count);
    if (swapchain_ctx == NULL) { return NULL; }

    return swapchain_ctx;
}

/*
* Cleanup
*/
void destroy_vulkan_context(VulkanContext *v_ctx) {
    destroy_swapchain_context(v_ctx->device, v_ctx->swapchain_ctx);
    vkDestroyDevice(v_ctx->device, NULL);
    if (ENABLE_VALIDAITON_LAYERS) {
        destroy_debug_utils_msg_ext(v_ctx->instance, v_ctx->debug_messenger, NULL);
    }

    vkDestroySurfaceKHR(v_ctx->instance, v_ctx->surface, NULL);
    vkDestroyInstance(v_ctx->instance, NULL);
    free(v_ctx);
}