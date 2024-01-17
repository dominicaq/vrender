#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "renderer/vulkan_context.h"
#include "renderer/pipeline.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
* Application Options
*/
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
const char *WINDOW_TITLE = "Vulkan Renderer";

/*
* Window
*/
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

void clean_up(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void print_glfw_version() {
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    printf("GLFW version: %d.%d.%d\n", major, minor, revision);
}

void debug_set_title(GLFWwindow *window, VkPhysicalDevice physical_device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    char debug_title[500];
    snprintf(debug_title, sizeof(debug_title), "Vulkan Renderer - %s", properties.deviceName);
    glfwSetWindowTitle(window, debug_title);
}
/*
* Application
*/
int main() {
    GLFWwindow *window = create_window(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    if (window == NULL) {
        fprintf(stderr, "failed to create GLFW window\n");
        return -1;
    }
    print_glfw_version();

    if (!glfwVulkanSupported()) {
        fprintf(stderr, "pc does not support vulkan\n");
        return -1;
    }

    VulkanContext *v_ctx = create_vulkan_context(window);
    debug_set_title(window, v_ctx->physical_device);

    // create_graphics_pipeline(v_ctx->device, "fart", "poop");

    printf("Running...\n");
    while(!glfwWindowShouldClose(window)) {
        // Input
        glfwPollEvents();

        // Render
        // vkBeginCommandBuffer(command_buffer, NULL);

        // vkEndCommandBuffer(command_buffer);
    }

    destroy_vulkan_context(v_ctx);
    clean_up(window);

    printf("Exit success\n");
    return 0;
}