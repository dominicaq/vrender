#include "pipeline.h"

#define FNAME_MAX 1024

const char *SHADER_BIN_DIR = "../../build/shaders/";

const int DYNAMIC_STATES_COUNT = 2;
const VkDynamicState DYNAMIC_STATES[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

void create_graphics_pipeline(VkDevice device, const char *fname_vert, const char *fname_frag) {
    VkShaderModule vertex_module = create_shader_module(device, fname_vert);
    if (vertex_module == NULL) {
        fprintf(stderr, "failed to create vertex module\n");
        return;
    }

    VkShaderModule frag_module = create_shader_module(device, fname_frag);
    if (frag_module == NULL) {
        fprintf(stderr, "failed to create fragment module\n");
        return;
    }

    VkPipelineShaderStageCreateInfo vert_create_info;
    vert_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_create_info.pNext = NULL;
    vert_create_info.flags = 0;
    vert_create_info.module = vertex_module;
    vert_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_create_info.pName = "main";
    vert_create_info.pSpecializationInfo = NULL;

    VkPipelineShaderStageCreateInfo frag_create_info;
    frag_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_create_info.pNext = NULL;
    frag_create_info.flags = 0;
    frag_create_info.module = frag_module;
    frag_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_create_info.pName = "main";
    frag_create_info.pSpecializationInfo = NULL;

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_create_info,
        frag_create_info
    };

    // Dynamic state creation
    VkPipelineDynamicStateCreateInfo dynamic_states_create_info;
    dynamic_states_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    dynamic_states_create_info.dynamicStateCount = DYNAMIC_STATES_COUNT;
    dynamic_states_create_info.pDynamicStates = DYNAMIC_STATES;
}

/*
* Rasterizer
*/
void rasterizer() {

}

/*
* Input assembler
*/
void input_assembly() {

}

/*
* Shader creation
*/
VkShaderModule create_shader_module(VkDevice device, const char *fname) {
    VkShaderModuleCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = NULL;
    create_info.flags = 0;

    // Create file path
    char f_path[FNAME_MAX] = {0};
    strcpy(f_path, SHADER_BIN_DIR);
    strcat(f_path, fname);

    size_t shader_code_size = 0;
    uint32_t *shader_code = read_file(f_path, &shader_code_size);
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