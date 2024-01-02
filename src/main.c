#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include "global_config.h"
#include "device.h"
#include "validation_layers.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

// Options
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
const char *WINDOW_TITLE = "Vulkan Renderer";

// Renderer
GLFWwindow *create_window(int width, int height, const char *title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "failed to create glfw window!\n");
        return NULL;
    }

    return window;
}

void clean_up(GLFWwindow *window, VkInstance instance, ValidationConfig *validation_config) {
    if (enable_validation_layers) {
        destroy_debug_utils_msg_ext(instance, validation_config->debug_messenger, NULL);
    }

    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
}

VkInstance create_instance(ValidationConfig *validation_config) {
    if (enable_validation_layers && !check_validation_layer_support(validation_config)) {
        fprintf(stderr, "validation layers requested, but not supported!\n");
        return NULL;
    }

    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Renderer";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.pNext = NULL;

    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    // Get extensions
    uint32_t extention_count = 0;
    const char **extension_names = get_required_extensions(&extention_count);
    if (extension_names == NULL) {
        return NULL;
    }

    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    create_info.enabledExtensionCount = extention_count;
    create_info.ppEnabledExtensionNames = extension_names;
    create_info.enabledLayerCount = 0;
    create_info.pNext = NULL;

    if (enable_validation_layers) {
        create_info.enabledLayerCount = validation_config->count;
        create_info.ppEnabledLayerNames = (const char* const*)validation_config->layers;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
        populate_debug_messenger_create_info(&debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }

    // Create instance
    VkInstance instance;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "failed to create VkInstance\n");
        return NULL;
    }

    return instance;
}

int main() {
    GLFWwindow *window = create_window(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    if (window == NULL) {
        fprintf(stderr, "failed to create GLFW window\n");
        return -1;
    }

    // Validation
    ValidationConfig validation_config;
    char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    validation_config.count = 1;
    validation_config.layers = validation_layers;
    validation_config.debug_messenger = VK_NULL_HANDLE;

    VkInstance instance = create_instance(&validation_config);
    if (instance == NULL) {
        fprintf(stderr, "failed to create VkInstance\n");
        return -1;
    }

    if (enable_validation_layers) {
        create_debug_messenger(instance, &validation_config.debug_messenger);
        if (validation_config.debug_messenger == VK_NULL_HANDLE) {
            fprintf(stderr, "debug messenger failed to create\n");
        }

        print_extensions();
        printf("Validation layers enabled\n");
    }

    VkDevice device = create_logical_device(instance);

    printf("Running...\n");
    // VkCommandBuffer command_buffer;
    while(!glfwWindowShouldClose(window)) {
        // Input
        glfwPollEvents();

        // Render
        // vkBeginCommandBuffer(command_buffer, NULL);

        // vkEndCommandBuffer(command_buffer);
    }

    vkDestroyDevice(device, NULL);
    clean_up(window, instance, &validation_config);
    printf("Exit success\n");
    return 0;
}