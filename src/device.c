#include "device.h"

VkDevice create_logical_device(VkInstance instance) {
    // Get all available physical devices
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (device_count == 0) {
        fprintf(stderr, "failed to find physical device\n");
        return NULL;
    }

    VkPhysicalDevice *physical_devices = malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &device_count, physical_devices);

    // Choose a physical device
    VkPhysicalDevice physical_device;
    for (int i = 0; i < device_count; ++i) {
        if (is_physical_device_suitable(physical_devices[i])) {
            physical_device = physical_devices[i];
            break;
        }
    }

    VkDevice device;
    VkDeviceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext = NULL;
    create_info.flags = 0;

    // Family Queue(s)
    uint32_t queue_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, NULL);
    if (queue_count == 0) {
        // No queue families found
        return NULL;
    }

    QueueFamilyIndices *indices = malloc(sizeof(QueueFamilyIndices));
    if (indices == NULL) {
        return NULL;
    }

    VkDeviceQueueCreateInfo *queue_create_infos = create_queue_family_create_infos(physical_device, queue_count, indices);
    if (queue_create_infos == NULL) {
        fprintf(stderr, "failed to create device queue create infos\n");
        return NULL;
    }

    create_info.queueCreateInfoCount = queue_count;
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

    if(vkCreateDevice(physical_device, &create_info, NULL, &device) != VK_SUCCESS) {
        fprintf(stderr, "failed to create logical device!\n");
        return NULL;
    }

    return device;
}

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
    if (enable_validation_layers) {
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
    if (enable_validation_layers) {
        extensions[required_count + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    *extension_count = total_count;
    return extensions;
}

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

void populate_queue_family_indices(VkQueueFamilyProperties *family_properties, uint32_t queue_family_count, QueueFamilyIndices *indices) {
    uint32_t total_queue_count = 0;
    for (int i = 0; i < queue_family_count; ++i) {
        total_queue_count += family_properties[i].queueCount;
    }

    // Get number of indicies for each family
    indices->graphics_count = 0;
    indices->transfer_count = 0;
    indices->compute_count = 0;
    for (int i = 0; i < total_queue_count; ++i) {
        if (family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            ++indices->graphics_count;
        } else if (family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            ++indices->transfer_count;
        } else if (family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            ++indices->compute_count;
        }
    }

    indices->graphics_indicies = malloc(sizeof(uint32_t) * indices->graphics_count);
    indices->transfer_indicies = malloc(sizeof(uint32_t) * indices->transfer_count);
    indices->compute_indicies = malloc(sizeof(uint32_t) * indices->compute_count);
}

VkDeviceQueueCreateInfo *create_queue_family_create_infos(VkPhysicalDevice physical_device, uint32_t queue_family_count, QueueFamilyIndices *indices) {
    VkQueueFamilyProperties *family_properties = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    if (family_properties == NULL) {
        // malloc failed
        return NULL;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, family_properties);

    // Create priority array
    uint32_t total_queue_count = 0;
    for (int i = 0; i < queue_family_count; ++i) {
        total_queue_count += family_properties[i].queueCount;
    }
    float *queue_priorities = malloc(sizeof(float) * total_queue_count);
    if (queue_priorities == NULL) {
        return NULL;
    }

    VkDeviceQueueCreateInfo *queue_create_infos = malloc(sizeof(VkDeviceQueueCreateInfo) * queue_family_count);
    if (queue_create_infos == NULL) {
        return NULL;
    }

    populate_queue_family_indices(family_properties, queue_family_count, indices);
    if (indices->graphics_indicies == NULL || indices->transfer_indicies == NULL || indices->compute_indicies == NULL) {
        return NULL;
    }

    // Create queue create infos
    uint32_t queue_index = 0;
    uint32_t graphics_index = 0, transfer_index = 0, compute_index = 0;
    for (int i = 0; i < queue_family_count; ++i) {
        if (!(family_properties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))) {
            continue;
        }

        // Prioritize different queues
        float base_priority = 0.5f;
        if (family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            base_priority = 1.0f;
            indices->graphics_indicies[graphics_index] = i;
            ++graphics_index;
        } else if (family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            base_priority = 0.75f;
            indices->transfer_indicies[transfer_index] = i;
            ++transfer_index;
        } else if (family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            base_priority = 0.5f;
            indices->compute_indicies[compute_index] = i;
            ++compute_index;
        }

        // Rank queue priorities
        uint32_t family_start_index = queue_index;
        uint32_t queue_count = family_properties[i].queueCount;
        for (int j = 0; j < queue_count; ++j) {
            // Assign priorities in decreasing order
            float priority_score = base_priority - (float)j / queue_count;
            if (priority_score < 0.0f) {
                priority_score = 0.0f;
            }

            queue_priorities[queue_index] = priority_score;
            ++queue_index;
        }

        VkDeviceQueueCreateInfo queue_create_info;
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.pNext = NULL;
        queue_create_info.flags = 0;
        queue_create_info.queueFamilyIndex = i;
        queue_create_info.queueCount = queue_count;
        queue_create_info.pQueuePriorities = &(queue_priorities[family_start_index]);

        queue_create_infos[i] = queue_create_info;
    }

    free(queue_priorities);
    free(family_properties);
    return queue_create_infos;
}

void destroy_family_queue_indicies(QueueFamilyIndices *queue_family) {
    free(queue_family->graphics_indicies);
    free(queue_family->transfer_indicies);
    free(queue_family->compute_indicies);
    free(queue_family);
}