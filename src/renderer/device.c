#include "device.h"

/*
* Constants
*/
#ifdef NDEBUG
    const bool ENABLE_VALIDAITON_LAYERS = false;
#else
    const bool ENABLE_VALIDAITON_LAYERS = true;
#endif

const char *VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"
};

const char *DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/*
* Context creation
*/
VkInstance create_instance() {
    if (ENABLE_VALIDAITON_LAYERS) {
        uint32_t validation_layer_count = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
        if (!check_validation_layer_support(validation_layer_count, VALIDATION_LAYERS)) {
            fprintf(stderr, "validation layers requested, but not supported!\n");
            return NULL;
        }
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
    const char **extension_names = get_required_instance_extensions(&extention_count);
    if (extension_names == NULL) {
        return NULL;
    }

    create_info.flags = 0;
    create_info.enabledExtensionCount = extention_count;
    create_info.ppEnabledExtensionNames = extension_names;
    create_info.enabledLayerCount = 0;
    create_info.pNext = NULL;

    if (ENABLE_VALIDAITON_LAYERS) {
        create_info.enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
        populate_debug_messenger_create_info(&debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }

    // Create instance
    VkInstance instance;
    if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS) {
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
VkPhysicalDevice get_physical_device(VkInstance instance, VkSurfaceKHR surface) {
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
        if (is_physical_device_suitable(physical_devices[i], surface)) {
            physical_device = physical_devices[i];
            break;
        }
    }

    free(physical_devices);
    return physical_device;
}

bool is_physical_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    // Check if required device extensions are within device extension properties
    uint32_t available_extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &available_extension_count, NULL);

    VkExtensionProperties *available_extensions = malloc(sizeof(VkExtensionProperties) * available_extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &available_extension_count, available_extensions);

    bool is_suitable = true;
    uint32_t required_count = sizeof(DEVICE_EXTENSIONS) / sizeof(DEVICE_EXTENSIONS[0]);
    for (int i = 0; i < required_count; ++i) {
        bool found = false;

        // Check if the required extension is in the list of available extensions
        for (int j = 0; j < available_extension_count; ++j) {
            if (strcmp(DEVICE_EXTENSIONS[i], available_extensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }

        // If the required extension is not found the device is not suitable
        if (!found) {
            is_suitable = false;
            break;
        }
    }

    SwapChainSupportDetails *details = query_swapchain_support_details(physical_device, surface);
    if (details == NULL) { return false; }

    bool supports_swap_chain = details->format_count > 0 && details->present_mode_count > 0;

    destroy_swapchain_support_details(details);
    free(available_extensions);
    return is_suitable && supports_swap_chain;
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
    if (ENABLE_VALIDAITON_LAYERS) {
        create_info.enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
    }

    // Extensions
    create_info.enabledExtensionCount = sizeof(DEVICE_EXTENSIONS) / sizeof(DEVICE_EXTENSIONS[0]);
    create_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS;

    // Physical device features
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);
    create_info.pEnabledFeatures = &device_features;

    VkDevice logical_device;
    if(vkCreateDevice(physical_device, &create_info, NULL, &logical_device) != VK_SUCCESS) {
        return NULL;
    }

    return logical_device;
}

/*
* Instance extension helpers
*/
VkExtensionProperties *get_available_instance_extensions(uint32_t *extension_count) {
    vkEnumerateInstanceExtensionProperties(NULL, extension_count, NULL);
    VkExtensionProperties *properties = malloc(sizeof(VkExtensionProperties) * (*extension_count));
    if (properties == NULL) {
        fprintf(stderr, "failed to alloc extension properties\n");
    }

    vkEnumerateInstanceExtensionProperties(NULL, extension_count, properties);
    return properties;
}

const char **get_required_instance_extensions(uint32_t *extension_count) {
    uint32_t required_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&required_count);
    if (glfw_extensions == NULL) {
        fprintf(stderr, "failed to retrieve GLFW extensions\n");
        return NULL;
    }

    uint32_t additional_count = 1;
    if (ENABLE_VALIDAITON_LAYERS) {
        ++additional_count;
    }

    uint32_t total_count = required_count + additional_count;
    const char **extensions = malloc((total_count) * sizeof(char*));
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
    indices->present_index = -1;
    for (int i = 0; i < family_count; ++i) {
        if (family_properties[i].queueCount == 0) {
            continue;
        }

        // Graphics family
        if (family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices->graphics_index = i;
        }

        // Check if index supports surface
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
        if (present_support == true) {
            indices->present_index = i;
        }

        // Found indices to use
        if (indices->graphics_index >= 0 && indices->present_index >= 0) {
            break;
        }
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

        // TODO: For the future can use multiple queues per family
        // for now use 1 queue, priority = 1.0f
        // queue_create_info.queueCount = family_properties[i].queueCount;
        // queue_create_info.pQueuePriorities = &(queue_priorities[family_start_index]);

        queue_create_infos[i] = queue_create_info;
    }

    return queue_create_infos;
}

/*
* Debugging
*/
void print_instance_extensions() {
    uint32_t required_count = 0;
    const char **required_extensions = get_required_instance_extensions(&required_count);
    printf("Required extenstions: \n");
    for (int i = 0; i < required_count; ++i) {
        printf("\t- %s\n", required_extensions[i]);
    }

    uint32_t available_count = 0;
    VkExtensionProperties *available_extensions = get_available_instance_extensions(&available_count);
    printf("Available instance extenstions: \n");
    for (int i = 0; i < available_count; ++i) {
        printf("\t- %s\n", available_extensions[i].extensionName);
    }

    free(required_extensions);
    free(available_extensions);
}

void print_physical_device_extensions(VkPhysicalDevice physical_device) {
    uint32_t available_extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &available_extension_count, NULL);

    VkExtensionProperties *available_extensions = malloc(sizeof(VkExtensionProperties) * available_extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, NULL, &available_extension_count, available_extensions);

    printf("Available physical device extenstions: \n");
    for (int i = 0; i < available_extension_count; ++i) {
        printf("\t- %s\n", available_extensions[i].extensionName);
    }
}