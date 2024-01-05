#include "device.h"

#ifdef NDEBUG
    const bool ENABLE_VALIDAITON_LAYERS = false;
#else
    const bool ENABLE_VALIDAITON_LAYERS = true;
#endif

const uint32_t VALIDATION_LAYER_COUNT = 1;
const char *VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"
};

/*
* Context creation
*/
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

        print_extensions();
        printf("Validation layers enabled\n");
    }

    // Pick physical device
    v_ctx->physical_device = get_physical_device(v_ctx->instance);
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

    free(family_properties);
    return v_ctx;
}

VkInstance create_instance() {
    if (ENABLE_VALIDAITON_LAYERS && !check_validation_layer_support(VALIDATION_LAYER_COUNT, VALIDATION_LAYERS)) {
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

    create_info.flags = 0;
    create_info.enabledExtensionCount = extention_count;
    create_info.ppEnabledExtensionNames = extension_names;
    create_info.enabledLayerCount = 0;
    create_info.pNext = NULL;

    if (ENABLE_VALIDAITON_LAYERS) {
        create_info.enabledLayerCount = VALIDATION_LAYER_COUNT;
        create_info.ppEnabledLayerNames = (const char* const*)VALIDATION_LAYERS;

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

VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow *window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        return NULL;
    }
    return surface;
}

/*
* Device creation
*/
VkPhysicalDevice get_physical_device(VkInstance instance) {
    // Get all available physical devices
    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
    if (physical_device_count == 0) { return VK_NULL_HANDLE; }

    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * physical_device_count);
    if (physical_devices == NULL) { return VK_NULL_HANDLE; }
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);

    // Choose a physical device
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    for (int i = 0; i < physical_device_count; ++i) {
        if (is_physical_device_suitable(physical_devices[i])) {
            physical_device = physical_devices[i];
            break;
        }
    }

    free(physical_devices);
    return physical_device;
}

VkDevice create_logical_device(VkPhysicalDevice physical_device, VkQueueFamilyProperties *family_properties, uint32_t family_count) {
    // Begin filling device struct
    VkDeviceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext = NULL;
    create_info.flags = 0;

    // Device queue families
    VkDeviceQueueCreateInfo *queue_create_infos = create_queue_family_create_infos(family_properties, family_count);
    if (queue_create_infos == NULL) {
        fprintf(stderr, "failed to create device queue create infos\n");
        return NULL;
    }
    create_info.queueCreateInfoCount = family_count;
    create_info.pQueueCreateInfos = queue_create_infos;

    // Layers
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = NULL;

    // Extensions
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = NULL;

    // Physical device features
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);
    create_info.pEnabledFeatures = &device_features;

    VkDevice logical_device;
    if(vkCreateDevice(physical_device, &create_info, NULL, &logical_device) != VK_SUCCESS) {
        fprintf(stderr, "failed to create logical device!\n");
        return NULL;
    }

    return logical_device;
}

/*
* Extension helpers
*/
bool is_physical_device_suitable(VkPhysicalDevice physical_device) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           device_features.geometryShader;
}

VkExtensionProperties *get_available_extensions(uint32_t *extension_count) {
    vkEnumerateInstanceExtensionProperties(NULL, extension_count, NULL);
    VkExtensionProperties *properties = malloc(sizeof(VkExtensionProperties) * (*extension_count));
    if (properties == NULL) {
        fprintf(stderr, "failed to alloc extension properties\n");
    }

    vkEnumerateInstanceExtensionProperties(NULL, extension_count, properties);
    return properties;
}

const char **get_required_extensions(uint32_t *extension_count) {
    uint32_t required_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&required_count);
    if (glfw_extensions == NULL) {
        fprintf(stderr, "failed to retrieve GLFW extensions\n");
        return NULL;
    }

    uint32_t additional_extensions = 1;
    if (ENABLE_VALIDAITON_LAYERS) {
        additional_extensions++;
    }

    uint32_t total_count = required_count + additional_extensions;
    const char **extensions = malloc((total_count) * sizeof(char *));
    if (extensions == NULL) {
        fprintf(stderr, "failed to alloc memory for extensions array\n");
        return NULL;
    }

    for (int i = 0; i < required_count; i++) {
        extensions[i] = glfw_extensions[i];
    }

    // Additional extensions
    extensions[required_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    if (ENABLE_VALIDAITON_LAYERS) {
        extensions[required_count + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    *extension_count = total_count;
    return extensions;
}

/*
* Queue families
*/
VkQueueFamilyProperties *get_queue_family_properties(VkPhysicalDevice physical_device, uint32_t *family_count) {
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, family_count, NULL);
    VkQueueFamilyProperties *family_properties = malloc(sizeof(VkQueueFamilyProperties) * (*family_count));
    if (family_properties == NULL) { return NULL; }
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, family_count, family_properties);
    return family_properties;
}

QueueFamilyIndices *get_unique_queue_family_indices(VkPhysicalDevice physical_device, VkSurfaceKHR surface, VkQueueFamilyProperties *family_properties, uint32_t family_count) {
    QueueFamilyIndices *indices = malloc(sizeof(QueueFamilyIndices));
    if (indices == NULL) { return NULL; }

    indices->graphics_index = -1;
    // TODO: Setup vkSurface
    indices->present_index = 2;

    for (int i = 0; i < family_count; ++i) {
        if (family_properties[i].queueCount == 0) {
            continue;
        }

        // Graphics family
        if (family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices->graphics_index = i;
        }

        // Check if index supports surface
        // VkBool32 present_support = false;
        // vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        // if (present_support == true) {
        //     indices->present_index = i;
        // }

        // // Found indices to use
        // if (indices->graphics_index >= 0 && indices->present_index >= 0) {
        //     break;
        // }
    }

    if (indices->graphics_index < 0 || indices->present_index < 0) {
        free(indices);
        return NULL;
    }
    return indices;
}

VkDeviceQueueCreateInfo *create_queue_family_create_infos(VkQueueFamilyProperties *family_properties, uint32_t family_count) {
    VkDeviceQueueCreateInfo *queue_create_infos = malloc(sizeof(VkDeviceQueueCreateInfo) * family_count);
    if (queue_create_infos == NULL) { return NULL; }

    // Create queue create infos
    float priority = 1.0f;
    for (int i = 0; i < family_count; ++i) {

        VkDeviceQueueCreateInfo queue_create_info;
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.pNext = NULL;
        queue_create_info.flags = 0;
        queue_create_info.queueFamilyIndex = i;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &priority;

        // TODO: For the future use multiple queues per family
        // for now use 1 queue, priority = 1.0f
        // queue_create_info.queueCount = family_properties[i].queueCount;
        // queue_create_info.pQueuePriorities = &(queue_priorities[family_start_index]);

        queue_create_infos[i] = queue_create_info;
    }

    return queue_create_infos;
}

/*
* Cleanup
*/
void destroy_vulkan_context(VulkanContext *v_ctx) {
    vkDestroyDevice(v_ctx->device, NULL);
    if (ENABLE_VALIDAITON_LAYERS) {
        destroy_debug_utils_msg_ext(v_ctx->instance, v_ctx->debug_messenger, NULL);
    }

    vkDestroySurfaceKHR(v_ctx->instance, v_ctx->surface, NULL);
    vkDestroyInstance(v_ctx->instance, NULL);
    free(v_ctx);
}

/*
* Debugging
*/
void print_extensions() {
    uint32_t required_count = 0;
    const char **required_extensions = get_required_extensions(&required_count);
    printf("Required extenstions: \n");
    for (int i = 0; i < required_count; ++i) {
        printf("\t- %s\n", required_extensions[i]);
    }

    uint32_t available_count = 0;
    VkExtensionProperties *available_extensions = get_available_extensions(&available_count);
    printf("Available extenstions: \n");
    for (int i = 0; i < available_count; ++i) {
        printf("\t- %s\n", available_extensions[i].extensionName);
    }

    free(required_extensions);
    free(available_extensions);
}
