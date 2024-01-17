#include "pipeline.h"

void create_graphics_pipeline(VkDevice device, const char *fname_vert, const char *fname_frag) {
    VkShaderModule vertex_module = create_shader_module(device, fname_vert);
    if (vertex_module == NULL) {
        fprintf(stderr, "failed to create vertex module\n");
        return;
    }

    VkShaderModule fragment_module = create_shader_module(device, fname_frag);
    if (fragment_module == NULL) {
        fprintf(stderr, "failed to create fragment module\n");
        return;
    }

}

VkShaderModule create_shader_module(VkDevice device, const char *fname) {
    VkShaderModuleCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = NULL;
    create_info.flags = 0;

    size_t shader_code_size = 0;
    uint32_t *shader_code = read_file(fname, &shader_code_size);
    if (shader_code == NULL) { return NULL; }
    create_info.codeSize = shader_code_size;
    create_info.pCode = shader_code;

    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, NULL, &shader_module) != VK_SUCCESS) {
        return NULL;
    }

    free(shader_code);
    return shader_module;
}

uint32_t *read_file(const char *fname, size_t *bytes_read) {
    int fsize = 0;
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "failed to open file: %s\n", fname);
        return NULL;
    }

    // Seek to end of file
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint32_t *buffer = malloc(sizeof(uint32_t) * (fsize + 1));
    if (buffer == NULL) { return NULL; }

    *bytes_read = fread(buffer, sizeof(uint32_t), fsize, fp);
    fclose(fp);

    if (bytes_read <= 0) {
        fprintf(stderr, "failed to read file: %s ", fname);
        return NULL;
    }

    // Null terminate buffer
    buffer[*bytes_read + 1] = '\0';
    return buffer;
}