#pragma once

#ifndef VK_X_TOOL
#define VK_X_TOOL

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#ifdef WIN32
#define EINLINE __forceinline
#elif define __GUNC__
#define EINLINE __attribute __((always_inline))
#else
#define EINLINE inline
#endif

EINLINE VkPipelineInputAssemblyStateCreateInfo& IAStateCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkBool32 primitiveRestartEnable = VK_FALSE) {
	VkPipelineInputAssemblyStateCreateInfo createInfo{
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};
	return createInfo;
}

#define DEFAULT_RASTER_INFO VkPipelineRasterizationStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE, VK_FALSE, 0, 0, 0, 1}

#define SET_VIEW(width, height) VkViewport{0, 0, (float)width, (float)height, 0, 1}

#define SET_SCI(width, height) VkRect2D{0, 0, width, height}

#define NO_MULTISAMPLE VkPipelineMultisampleStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0, VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1, nullptr, VK_FALSE, VK_FALSE}

#define ATTACH_NO_BLEND VkPipelineColorBlendAttachmentState{VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT}

#define DEFALUT_BLEND_STATE(blendStates) 

#endif
