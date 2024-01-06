// #include "swapchain.h"

// #define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// /*
// * Swapchain creation
// */
// SwapchainContext *create_swapchain_context(VulkanContext *v_ctx, GLFWwindow *window) {
//     SwapchainContext *swapchain_ctx = malloc(sizeof(SwapchainContext));
//     if (swapchain_ctx == NULL) { return NULL; }

//     SwapChainSupportDetails *details = query_swapchain_support_details(v_ctx->physical_device, v_ctx->surface);
//     if (details == NULL) { return NULL; }

//     uint32_t image_count = details->capabilities.minImageCount + NO_BUFFERING;
//     if (details->capabilities.maxImageCount > 0 && image_count > details->capabilities.maxImageCount) {
//         image_count = details->capabilities.maxImageCount;
//     }

//     VkSurfaceFormatKHR surface_format = get_swapchain_surface_format(details->formats, details->format_count);
//     VkPresentModeKHR present_mode = get_swapchain_present_mode(details->present_modes, details->present_mode_count);
//     VkExtent2D extent = get_swapchain_extent(details->capabilities, window);

//     VkSwapchainCreateInfoKHR create_info;
//     create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//     create_info.pNext = NULL;
//     create_info.flags = 0;
//     create_info.surface = v_ctx->surface;

//     // Swapchain image
//     create_info.minImageCount = image_count;
//     create_info.imageFormat = surface_format.format;
//     create_info.imageColorSpace = surface_format.colorSpace;
//     create_info.imageExtent = extent;
//     create_info.imageArrayLayers = 1;
//     create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

//     // Queue indices data
//     create_info.queueFamilyIndexCount = 0;
//     create_info.pQueueFamilyIndices = NULL;
//     create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

//     uint32_t graphics_family = v_ctx->indices->graphics_index;
//     uint32_t present_family = v_ctx->indices->present_index;
//     uint32_t queueFamilyIndices[2] = {graphics_family, present_family};
//     if (graphics_family != present_family) {
//         create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//         create_info.queueFamilyIndexCount = 2;
//         create_info.pQueueFamilyIndices = queueFamilyIndices;
//     }

//     // Presentation
//     create_info.preTransform = details->capabilities.currentTransform;
//     create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//     create_info.presentMode = present_mode;
//     create_info.clipped = VK_TRUE;

//     // Swapchain
//     create_info.oldSwapchain = VK_NULL_HANDLE;
//     if (vkCreateSwapchainKHR(v_ctx->device, &create_info, NULL, &swapchain_ctx->swapchain) != VK_SUCCESS) {
//         return NULL;
//     }

//     destroy_swapchain_support_details(details);
//     return swapchain_ctx;
// }

// VkSurfaceFormatKHR get_swapchain_surface_format(VkSurfaceFormatKHR *formats, uint32_t format_count) {
//     for (int i = 0; i < format_count; ++i) {
//         if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB
//             && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//             return formats[i];
//         }
//     }

//     return formats[0];
// }

// VkPresentModeKHR get_swapchain_present_mode(VkPresentModeKHR *present_modes, uint32_t present_mode_count) {
//     for (int i = 0; i < present_mode_count; ++i) {
//         if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
//             return present_modes[i];
//         }
//     }
//     return present_modes[0];
// }

// VkExtent2D get_swapchain_extent(VkSurfaceCapabilitiesKHR capabilities, GLFWwindow *window) {
//     if (capabilities.currentExtent.width != INT_MAX) {
//         return capabilities.currentExtent;
//     }

//     int width = 0, height = 0;
//     glfwGetFramebufferSize(window, &width, &height);
//     width = CLAMP(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//     height = CLAMP(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

//     VkExtent2D actual_extent = {
//         (uint32_t)width,
//         (uint32_t)height
//     };

//     return actual_extent;
// }

// /*
// * Swapchain support detail
// */
// SwapChainSupportDetails *query_swapchain_support_details(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
//     SwapChainSupportDetails *details = malloc(sizeof(SwapChainSupportDetails));
//     if (details == NULL) { return NULL; }

//     vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

//     uint32_t format_count = 0;
//     vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);
//     details->formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
//     if (details->formats == NULL) { return NULL; }

//     details->format_count = format_count;
//     vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details->formats);

//     uint32_t present_mode_count = 0;
//     vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
//     details->present_modes = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
//     if (details->present_modes == NULL) { return NULL; }

//     details->present_mode_count = present_mode_count;
//     vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details->present_modes);

//     return details;
// }

// /*
// * Cleanup
// */
// void destroy_swapchain_context(VkDevice device, SwapchainContext *swapchain_ctx) {
//     vkDestroySwapchainKHR(device, swapchain_ctx->swapchain, NULL);
//     free(swapchain_ctx->images);
//     free(swapchain_ctx);
// }

// void destroy_swapchain_support_details(SwapChainSupportDetails *details) {
//     free(details->formats);
//     free(details->present_modes);
//     free(details);
// }