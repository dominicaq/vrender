#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include "device.h"
#include "validation_layers.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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

void clean_up(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    GLFWwindow *window = create_window(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    if (window == NULL) {
        fprintf(stderr, "failed to create GLFW window\n");
        return -1;
    }

    VulkanDevice *device = create_vulkan_device();

    printf("Running...\n");
    while(!glfwWindowShouldClose(window)) {
        // Input
        glfwPollEvents();

        // Render
        // vkBeginCommandBuffer(command_buffer, NULL);

        // vkEndCommandBuffer(command_buffer);
    }

    destroy_vulkan_device(device);
    clean_up(window);
    printf("Exit success\n");
    return 0;
}