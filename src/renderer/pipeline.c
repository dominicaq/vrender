#include "pipeline.h"

#define FNAME_MAX 1024

const int DYNAMIC_STATES_COUNT = 2;
const VkDynamicState DYNAMIC_STATES[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

/*
* Pipeline creation
*/
VkPipeline create_graphics_pipeline(VkDevice device, SwapchainContext *swapchain_ctx, const char *fname_vert, const char *fname_frag) {
    VkShaderModule vertex_module = create_shader_module(device, fname_vert);
    if (vertex_module == NULL) {
        fprintf(stderr, "failed to create vertex module\n");
        return NULL;
    }

    VkShaderModule frag_module = create_shader_module(device, fname_frag);
    if (frag_module == NULL) {
        fprintf(stderr, "failed to create fragment module\n");
        return NULL;
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
    VkPipelineDynamicStateCreateInfo dynamics_create_info;
    dynamics_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamics_create_info.dynamicStateCount = DYNAMIC_STATES_COUNT;
    dynamics_create_info.pDynamicStates = DYNAMIC_STATES;

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_create_info;
    vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_create_info.pNext = NULL;
    vertex_input_create_info.flags = 0;
    vertex_input_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_create_info.pVertexAttributeDescriptions = NULL;
    vertex_input_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_create_info.pVertexBindingDescriptions = NULL;

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info;
    input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_create_info.pNext = NULL;
    input_assembly_create_info.flags = 0;
    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

    // Viewport state creation
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain_ctx->extent.width;
    viewport.height = swapchain_ctx->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = swapchain_ctx->extent;

    VkPipelineViewportStateCreateInfo viewport_create_info;
    viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_create_info.pNext = NULL;
    viewport_create_info.flags = 0;
    viewport_create_info.viewportCount = 1;
    viewport_create_info.pViewports = &viewport;
    viewport_create_info.scissorCount = 1;
    viewport_create_info.pScissors = &scissor;

    // Rasterization
    VkPipelineRasterizationStateCreateInfo rasterizer_create_info;
    rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_create_info.pNext = NULL;
    rasterizer_create_info.flags = 0;
    rasterizer_create_info.rasterizerDiscardEnable = VK_FALSE;

    // Triangles
    rasterizer_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // Depth
    rasterizer_create_info.depthClampEnable = VK_FALSE;
    rasterizer_create_info.depthBiasEnable = VK_FALSE;
    rasterizer_create_info.depthBiasConstantFactor = 0.0f;
    rasterizer_create_info.depthBiasClamp = 0.0f;
    rasterizer_create_info.depthBiasSlopeFactor = 0.0f;

    rasterizer_create_info.lineWidth = 1.0f;

    // Anti-aliasing
    VkPipelineMultisampleStateCreateInfo multiple_sample_create_info;
    multiple_sample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiple_sample_create_info.pNext = NULL;
    multiple_sample_create_info.flags = 0;
    multiple_sample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiple_sample_create_info.sampleShadingEnable = VK_FALSE;
    multiple_sample_create_info.minSampleShading = 1.0f;
    multiple_sample_create_info.pSampleMask = NULL;
    multiple_sample_create_info.alphaToCoverageEnable = VK_FALSE;
    multiple_sample_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attacthment;
    color_blend_attacthment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    color_blend_attacthment.blendEnable = VK_FALSE;
    color_blend_attacthment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attacthment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attacthment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attacthment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attacthment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attacthment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_create_info;
    color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_create_info.pNext = NULL;
    color_blend_create_info.flags = 0;
    color_blend_create_info.logicOpEnable = VK_FALSE;
    color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_create_info.attachmentCount = 1;
    color_blend_create_info.pAttachments = &color_blend_attacthment;
    color_blend_create_info.blendConstants[0] = 0.0f;
    color_blend_create_info.blendConstants[1] = 0.0f;
    color_blend_create_info.blendConstants[2] = 0.0f;
    color_blend_create_info.blendConstants[3] = 0.0f;

    // Pipeline creation
    VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = NULL;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = NULL;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = NULL;

    VkPipelineLayout pipeline_layout;
    if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, NULL, &pipeline_layout) != VK_SUCCESS) {
        fprintf(stderr, "failed to create pipeline layout\n");
        return NULL;
    }

    VkGraphicsPipelineCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.stageCount = 2;
    create_info.pStages = shader_stages;
    create_info.pVertexInputState = &vert_create_info;
    create_info.pInputAssemblyState = &input_assembly_create_info;
    create_info.pViewportState = &viewport_create_info;
    create_info.pRasterizationState = &rasterizer_create_info;
    create_info.pMultisampleState = &multiple_sample_create_info;
    create_info.pDepthStencilState = NULL;
    create_info.pColorBlendState = &color_blend_create_info;
    create_info.pDynamicState = &DYNAMIC_STATES;

    VkPipeline graphics_pipeline;
    if (vkCreateGraphicsPipelines(device, NULL, 1, &create_info, NULL, &graphics_pipeline) != VK_SUCCESS) {
        fprintf(stderr, "failed to create graphics pipeline(s)\n");
        return NULL;
    }

    return graphics_pipeline;
}


/*
* Render pass
*/
VkRenderPass create_render_pass(VkDevice device, SwapchainContext *swapchain_ctx) {
    VkAttachmentDescription color_attachment;
    color_attachment.flags = 0;
    color_attachment.format = swapchain_ctx->image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.layout = 0;
    color_attachment_ref.attachment = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;

    VkRenderPassCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.flags = 0;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;
    create_info.dependencyCount = 0;
    create_info.pDependencies = NULL;

    VkRenderPass renderpass;
    if (vkCreateRenderPass(device, &create_info, NULL, &renderpass) != VK_SUCCESS) {
        return NULL;
    }

    return renderpass;
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
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "failed to open file: %s\n", fname);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint32_t *buffer = malloc(fsize);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }

    *bytes_read = fread(buffer, 1, fsize, fp);
    fclose(fp);

    if (*bytes_read != fsize) {
        fprintf(stderr, "failed to read file: %s\n", fname);
        free(buffer);
        return NULL;
    }

    return buffer;
}