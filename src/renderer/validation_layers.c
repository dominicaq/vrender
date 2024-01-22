#include "validation_layers.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL msg_debugger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void* p_user_data) {

    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf(stderr, "[WARNING] %s\n", p_callback_data->pMessage);
    } else if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        fprintf(stderr, "[ERROR] %s\n", p_callback_data->pMessage);
    }

    return VK_FALSE;
}

VkDebugUtilsMessengerEXT create_debug_messenger(VkInstance instance) {
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(&create_info);

    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    if (create_debug_utils_msg_ext(instance, &create_info, NULL, &debug_messenger) != VK_SUCCESS) {
        fprintf(stderr, "failed to set up debug messenger\n");
        return VK_NULL_HANDLE;
    }

    return debug_messenger;
}

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info) {
    create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info->messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info->messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info->pfnUserCallback = msg_debugger_callback;
    create_info->pUserData = NULL;
    create_info->pNext = NULL;
    create_info->flags = 0;
}

bool check_validation_layer_support(uint32_t num_validation_layers, const char **validation_layers) {
    uint32_t instance_layer_count;
    vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);

    VkLayerProperties *available_layers = malloc(sizeof(VkLayerProperties) * instance_layer_count);
    if (available_layers == NULL) {
        fprintf(stderr, "failed to allocate memory for available layers\n");
        return false;
    }

    vkEnumerateInstanceLayerProperties(&instance_layer_count, available_layers);

    bool supports_layers = true;
    for (int i = 0; i < num_validation_layers; ++i) {
        bool layer_found = false;
        for (int j = 0; j < instance_layer_count; ++j) {
            if (strcmp(validation_layers[i], available_layers[j].layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            fprintf(stderr, "Validation layer not found: %s\n", validation_layers[i]);
            supports_layers = false;
            break;
        }
    }

    free(available_layers);
    return supports_layers;
}

VkResult create_debug_utils_msg_ext(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, p_create_info, p_allocator, p_debug_messenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_msg_ext(VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator) {

    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debug_messenger, p_allocator);
    }
}

void print_available_layers() {
    uint32_t instance_layer_count;
    vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);

    VkLayerProperties *available_layers = malloc(sizeof(VkLayerProperties) * instance_layer_count);
    if (available_layers == NULL) {
        fprintf(stderr, "failed to allocate memory for available layers\n");
    }

    vkEnumerateInstanceLayerProperties(&instance_layer_count, available_layers);

    printf("Available Validation Layers:\n");
    for (int i = 0; i < instance_layer_count; ++i) {
        printf("\t%s\n", available_layers[i].layerName);
    }
    free(available_layers);
}