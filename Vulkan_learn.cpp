#include "VKBase.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

using namespace std;

int main() {
	// glfw窗口初始化
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* glfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "window", nullptr, nullptr);
	NativeSurface natSurf = { WINDOW_WIDTH , WINDOW_HEIGHT , glfwWindow };

	EngineInitInfo initInfo = { "Vulkan Test", "Engine"};

	// 初始化引擎
	auto factory = ComPtr(Factory::createFactory(&initInfo));

	//factory->queryDeviceMemory();

	// 获得设备句柄
	auto device = ComPtr(factory->createDevice(0));
	
	// 创建窗口表面
	auto surface = ComPtr(device->createSurface(&natSurf));

	// 查询所有支持的表面格式
	auto formats = surface->querySurfaceFormats();

	// 查询所有支持的表面呈现方式
	auto presentModes = surface->queryPresentMode();

	// 查询表面支持的特性和能力
	VkSurfaceCapabilitiesKHR surfCap;
	surface->querySurfaceCapabilities(&surfCap);

	// 如果窗口宽高大于表面允许最大值，报错
	if (WINDOW_WIDTH > surfCap.maxImageExtent.width) {
		logE("Window width is greater than the maximum surface width.\n");
		assert(0);
	}
	if (WINDOW_HEIGHT > surfCap.maxImageExtent.height) {
		logE("Window height is greater than the maximum surface height.\n");
		assert(0);
	}

	// 获得图形队列
	auto cmdQueue = ComPtr(device->getCommandQueue(Graphics, 0));

	// 创建命令分配器
	auto cmdAlloc = ComPtr(device->createCommandAllocator(Graphics));

	// 创建交换链
	uint32_t swBufCount = 3;
	SwapchainCreateInfo spcCreateInfo;
	spcCreateInfo.width = WINDOW_WIDTH;
	spcCreateInfo.height = WINDOW_HEIGHT;
	spcCreateInfo.format = formats[0];
	// 如果不支持VK_PRESENT_MODE_MAILBOX_KHR模式就用VK_PRESENT_MODE_FIFO_KHR模式
	for (auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			goto here;
		}
	}
	spcCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
here:;
	// 交换链缓冲数量默认为3，如果最小值大于3就按最小值来，最大值小于3就按最大值来
	if (surfCap.minImageCount > 3) {
		swBufCount = surfCap.minImageCount;
	} else if (surfCap.maxImageCount < 3) {
		swBufCount = surfCap.maxImageCount;
	}
	spcCreateInfo.buffersCount = swBufCount;
	auto swapchain = ComPtr(surface->createSwapChain(&spcCreateInfo, cmdQueue.get()));

	// 获得交换链缓冲和缓冲的视图
	vector<Image*> swapchainBuffers(swBufCount);
	for (uint32_t i = 0; i < swBufCount; i++) swapchainBuffers[i] = swapchain->getBuffer(i);
	vector<shared_ptr<ImageView>> swapchanBufferViews(3);
	for (uint32_t i = 0; i < swBufCount; i++) swapchanBufferViews[i] = ComPtr(device->createSwapchainImageView(swapchainBuffers[i], swapchain->getFormat()));

	// 创建渲染流程
	shared_ptr<RenderPass> renderPass;
	{
		// 描述子流程绑定的附着信息
		EVECTOR<VkAttachmentDescription> attachments(1);
		attachments[0].format = swapchain->getFormat();
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;							// 指定采样数
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;					// 渲染前对颜色和深度附着的操作
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;					// 渲染后对颜色和深度附着的操作
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				// 指定渲染前图像布局方式，此处不关心
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;			// 指定渲染后图像布局方式，用于被交换链呈现
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			// 渲染前对模板的操作
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		// 渲染后对模板的操作

		// 子流程信息
		EVECTOR<VkSubpassDescription> subpassDescs(1);
		// 描述子流程使用的附着信息
		EVECTOR<VkAttachmentReference> colorAttachmentRefs(1);
		colorAttachmentRefs[0].attachment = 0;												// 指定要引用的附着在附着描述结构体中的索引
		colorAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			// 指定引用附着的布局方式
		// 描述子流程
		subpassDescs[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescs[0].colorAttachmentCount = colorAttachmentRefs.size();
		subpassDescs[0].pColorAttachments = colorAttachmentRefs.data();
		subpassDescs[0].pInputAttachments;
		subpassDescs[0].pResolveAttachments;
		subpassDescs[0].pDepthStencilAttachment;

		// 描述子流程依赖
		EVECTOR<VkSubpassDependency> dependencies(1);
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;				// 指定被依赖的子流程索引，此处设置为隐含的子流程（表示渲染流程开始前的子流程）
		dependencies[0].dstSubpass = 0;									// 依赖的子流程索引，0是之前创建的子流程的索引。为避免循环依赖，dstSubPass设置的值必须始终大于srcSubpass
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// 指定需要等待的管线阶段，此处等待交换链结束
		dependencies[0].srcAccessMask = 0;												// 指定子流程即将执行的操作类型
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;								// 指定需要等待的管线阶段，设置为等待颜色附着的输出
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// 指定子流程即将进行的操作，设置为对颜色附着读写

		renderPass = ComPtr(device->createRenderPass(&attachments, &subpassDescs, &dependencies));
	}

	// 创建流水线渲染对象
	shared_ptr<GraphicsPipeline> gfxPipe;
	{
		// 创建着色器
		auto vs = ComPtr(device->readShaderFile("D:\\code\\Vulkan_learn\\Vulkan_learn\\vert.spv"));
		auto fs = ComPtr(device->readShaderFile("D:\\code\\Vulkan_learn\\Vulkan_learn\\frag.spv"));

		// 创建流水线阶段
		EVECTOR<ShaderStage> shaderStages(2);
		shaderStages[0].shaderFile = vs.get();
		shaderStages[0].stage = VertexShaderStage;
		shaderStages[1].shaderFile = fs.get();
		shaderStages[1].stage = FragmentShaderStage;

		// 创建管线布局
		auto layout = ComPtr(device->createPipelineLayout());

		// 描述顶点数据
		VkPipelineVertexInputStateCreateInfo vtxDescCreateInfo = {};
		vtxDescCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vtxDescCreateInfo.vertexBindingDescriptionCount = 0;
		vtxDescCreateInfo.vertexAttributeDescriptionCount = 0;

		// 定义输入装配
		auto iAStateCreateInfo = IAStateCreateInfo();

		// 设置视口和裁剪矩形
		auto viewport = SET_VIEW(WINDOW_WIDTH, WINDOW_HEIGHT);
		auto scissor = SET_SCI(WINDOW_WIDTH, WINDOW_HEIGHT);
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// 设置光栅化状态
		auto rasterState = DEFAULT_RASTER_INFO;

		// 设置多重采样状态，依赖GPU特性
		auto multisampling = NO_MULTISAMPLE;

		// 设置混合模式
		// 配置每个帧缓冲
		EVECTOR<VkPipelineColorBlendAttachmentState> attachBlendStates(1);
		attachBlendStates[0] = ATTACH_NO_BLEND;
		// 配置全局混合模式
		VkPipelineColorBlendStateCreateInfo blendStates = {};
		blendStates.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendStates.logicOpEnable = VK_FALSE;					// 控制是否使用逻辑运算
		blendStates.logicOp = VK_LOGIC_OP_COPY;
		blendStates.attachmentCount = attachBlendStates.size();
		blendStates.pAttachments = attachBlendStates.data();
		blendStates.blendConstants[0] = 0;
		blendStates.blendConstants[1] = 0;
		blendStates.blendConstants[2] = 0;
		blendStates.blendConstants[3] = 0;

		PipeCreateInfo pipeInfo;
		pipeInfo.vertexInputState = &vtxDescCreateInfo;
		pipeInfo.inputAssemblyState = &iAStateCreateInfo;
		pipeInfo.viewportState = &viewportState;
		pipeInfo.rasterizationState = &rasterState;
		pipeInfo.multisampleState = &multisampling;
		pipeInfo.depthStencilState = nullptr;
		pipeInfo.colorBlendState = &blendStates;
		pipeInfo.dynamicState = nullptr;

		gfxPipe = ComPtr(device->createGraphicsPipeline(pipeInfo, shaderStages, layout.get(), renderPass.get()));
	}

	// 创建命令队列
	vector<shared_ptr<GraphicsCommandList>> cmdLists(swBufCount);
	for (auto& cmdList : cmdLists) {
		cmdList = ComPtr(cmdAlloc->createGraphicsCommandList());
	}

	// 创建帧缓冲
	vector<shared_ptr<FrameBuffer>> frambuffers(swBufCount);
	for (uint32_t i = 0; i < swBufCount; i++) {
		EVECTOR<ImageView*> attachments(1);
		attachments[0] = swapchanBufferViews[i].get();
		frambuffers[i] = ComPtr(device->createFrameBuffer(renderPass.get(), attachments, WINDOW_WIDTH, WINDOW_HEIGHT));
	}

	// 记录渲染命令
	for (uint32_t i = 0; i < swBufCount; i++) {
		// 开始记录命令
		cmdLists[i]->beginRecord();

		// 开始执行一个渲染流程
		VkRect2D drawRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		EVECTOR<VkClearValue> clearColors = { {0, 0, 0, 1} };
		cmdLists[i]->beginRenderPass(renderPass.get(), frambuffers[i].get(), drawRect, clearColors);

		// 将图形流水线对象绑定到渲染流程上
		cmdLists[i]->setGraphicsPipeline(gfxPipe.get());

		// 绘制
		cmdLists[i]->draw(3, 1, 0, 0);

		// 结束执行本次渲染流程
		cmdLists[i]->endRenderPass();

		// 结束记录命令
		cmdLists[i]->endRecord();
	}

	// 创建信号量
	auto imgAvailSem = ComPtr(device->createSemaphore());			// 表示缓冲区可被写入
	auto renderFinSem = ComPtr(device->createSemaphore());			// 表示渲染结束，可以呈现
	
	vector<int> vertices(100, 1);
	//auto resource = ComPtr(device->createResource(vertices.size() + 100, UploadHeap));
	//auto buffer = ComPtr(resource->createVertexBuffer(vertices.size()));
	auto buffer = ComPtr(device->createVertexBuffer(UploadHeap, vertices.size()));
	buffer->copy(vertices.data());

	int i = 0;
	while (1) {
		// 从交换链获取图像
		uint32_t imgIdx = swapchain->getNextImageIndex(imgAvailSem.get());

		EVECTOR<ExecuteInfo> executeInfos(1);
		executeInfos[0].commandLists.push_back(cmdLists[imgIdx].get());
		executeInfos[0].waitSemaphores.push_back(imgAvailSem.get());
		executeInfos[0].signalSemaphore.push_back(renderFinSem.get());
		executeInfos[0].waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
		cmdQueue->execute(executeInfos);

		EVECTOR<Semaphore*> semaphores(1);
		semaphores[0] = renderFinSem.get();
		swapchain->present(imgIdx, &semaphores);

		device->waitIdle();
	}

	imgAvailSem.reset();
	renderFinSem.reset();

	gfxPipe.reset();
	renderPass.reset();

	for (uint32_t i = 0; i < swapchanBufferViews.size(); i++) {
		swapchanBufferViews[i].reset();
	}

	for (auto cmdList : cmdLists) {
		cmdList.reset();
	}
	cmdAlloc.reset();

	frambuffers.clear();

	swapchain.reset();
	device.reset();
	factory.reset();
}