#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
//#define USE_THIS
#ifdef USE_THIS

//#define VK_USE_PLATFORM_WIN32_KHR
//#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>

#pragma 

using namespace std;

#define DDE
#ifdef DD
#define logE(...) printf("Error: "__VA_ARGS__ );
#define logI(...) printf("Info: "__VA_ARGS__ );
#else
#define logE(...) printf(__VA_ARGS__ );
#define logI(...) printf(__VA_ARGS__ );
#endif

#define CHECK_VK(func) { VkResult ret = func; if (ret != VK_SUCCESS) { cout << " Failed in File: " << THE_FILE << " Line: " << THE_LINE << endl; checkErrCode(ret); }}

#define THE_LINE __LINE__
#define THE_FILE __FILE__

// 用于VK_EXT_debug_tuils校验层扩展的回调函数
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
	VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,				// 指定了消息级别
	VkDebugUtilsMessageTypeFlagsEXT msgType,
	const VkDebugUtilsMessengerCallbackDataEXT* callBackData,
	void* userData)
{
	string dbgStr;
	switch (msgSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	{
		dbgStr += "Vulkan diagnostic Information: ";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	{
		dbgStr += "Vulkan resource creation: ";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
	{
		dbgStr += "Vulkan warning: ";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
	{
		dbgStr += "Vulkan error: ";
		break;
	}
	default:
	{
		dbgStr += "Unkwon severity " + to_string(msgSeverity) + " :";
	}
	}

	switch (msgType)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
	{
		dbgStr += "General event:\n";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
	{
		dbgStr += "Validated error:\n";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
	{
		dbgStr += "Possible performance issues:\n";
		break;
	}
	default:
	{
		dbgStr += "Unkown message type " + to_string(msgType) + " :\n";
	}
	}

	dbgStr += callBackData->pMessage;

	cout << dbgStr << "\n" << endl;

	return VK_FALSE;
}

// 检查错误码
void checkErrCode(VkResult ret)
{
#define VK_ERROR(str) printf("Vulkan Error:"str);
	switch (ret)
	{
	case VK_ERROR_LAYER_NOT_PRESENT:
	{
		VK_ERROR("The layer is not present.");
		break;
	}
	case VK_ERROR_EXTENSION_NOT_PRESENT:
	{
		VK_ERROR("Extension is not present.");
		break;
	}
	default:
	{
		VK_ERROR("Error code: %d", ret);
	}
	}
}

// 启用VK_EXT_debug_tuils校验层扩展并注册回调函数
bool setupDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT* callback)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =														// 指定回调函数的处理消息级别
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =															// 指定回调函数处理的消息类型
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallBack;										// 注册回调函数
	createInfo.pUserData = nullptr;													// 向回调函数传递用户自己定义的数据

	// 查找vkCreateDebugUtilsMessengerEXT函数
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func == nullptr)
	{
		logE("Failed to get function named vkCreateDebugUtilsMessengerEXT\n");
		return false;
	}

	// 注册回调函数
	CHECK_VK(func(instance, &createInfo, nullptr, callback));

	return true;
}

bool setdownDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT callback)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func == nullptr)
	{
		logE("Failed to get function named vkDestroyDebugUtilsMessengerEXT\n");
		return false;
	}
	func(instance, callback, nullptr);
	return true;
}

// 判断一个显卡是否满足要求（目前没啥用，都能满足）
bool isDeviceSuitable(VkPhysicalDevice device)
{
	// 获得设备属性
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	logI("Device name: %s\n", deviceProperties.deviceName);
	logI("Device driver version: %u\n", deviceProperties.driverVersion);

	// 获得设备支持特性
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return true;
}

vector<char> readShaderFile(const string& filepath)
{
	vector<char> ret;
	ifstream file(filepath, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		logE("Failed to open %s\n", filepath.c_str());
		return  ret;
	}

	size_t fileSize = (size_t)file.tellg();
	vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return move(buffer);
}

VkShaderModule createShaderModule(VkDevice vkDevice, const vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	CHECK_VK(vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule));

	return shaderModule;
}

int main()
{
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;

	// GLFW初始化部分
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "window", nullptr, nullptr);

	// Vulkan初始化部分
		// VkApplicationInfo用于告知驱动使用了什么引擎、是什么程序等
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No_Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstance vkInstance;

#define OPEN_DEBUG
#ifndef  OPEN_DEBUG

	// 必须结构体，用于高速vk驱动要用的全局扩展、校验层等信息
	// 不使用任何扩展，不使用任何校验层
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;
	createInfo.enabledLayerCount = 0;

	// 初始化Vulkan实例，指定了一些驱动程序需要使用的应用程序信息
	CHECK_VK(vkCreateInstance(&createInfo, nullptr, &vkInstance));
#else
	// 获得支持的扩展
	uint32_t extensionCout = 0;
	CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCout, nullptr));
	vector<VkExtensionProperties> extensions(extensionCout);
	CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCout, extensions.data()));
	cout << "Extensions: " << endl;
	for (const auto& extension : extensions)
	{
		cout << extension.extensionName << endl;
	}

	// 获得支持的校验层
	uint32_t layerCount;
	CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
	vector<VkLayerProperties> availLayers(layerCount);
	CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, availLayers.data()));
	vector<const char*> usedLayers;
	cout << "Available layers:" << endl;
	for (const auto& layerProp : availLayers)
	{
		cout << layerProp.layerName << endl;
		if (string("VK_LAYER_KHRONOS_validation") == string(layerProp.layerName))
		{
			usedLayers.push_back(layerProp.layerName);
		}
	}

	// 启用指定扩展
	vector<const char*> usedExt;
	// usedExt.push_back("VK_EXT_debug_report");
	usedExt.push_back("VK_EXT_debug_utils");
	usedExt.push_back("VK_KHR_win32_surface");
	usedExt.push_back("VK_KHR_surface");

	// 必须结构体，用于高速vk驱动要用的全局扩展、校验层等信息
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = usedExt.size();
	createInfo.ppEnabledExtensionNames = usedExt.data();
	createInfo.enabledLayerCount = usedLayers.size();
	createInfo.ppEnabledLayerNames = usedLayers.data();

	// 初始化Vulkan实例，指定了一些驱动程序需要使用的应用程序信息
	CHECK_VK(vkCreateInstance(&createInfo, nullptr, &vkInstance));

	// 注册DEBUG回调
	VkDebugUtilsMessengerEXT dbgUtilMsgExt;
	if (!setupDebugExt(vkInstance, &dbgUtilMsgExt))
	{
		logE("Failed to setupDebugExt\n");
	}
#endif

	// 创建窗口表面
	VkSurfaceKHR surface;
	CHECK_VK(glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &surface));

	// 选择物理设备
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount == 0)
	{
		logE("Display devices is 0 !");
	}
	else
	{
		logI("Device count: %u\n", physicalDeviceCount);
	}
	vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());
	for (int i = 0; i < physicalDeviceCount; i++)
	{
		if (isDeviceSuitable(physicalDevices[i]))
		{
			physicalDevice = physicalDevices[i];
		}
	}

	// 选择GRAPHICS队列族
	uint32_t queueFamliyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamliyCount, nullptr);
	vector<VkQueueFamilyProperties> queueFamilies(queueFamliyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamliyCount, queueFamilies.data());
	int graphcsQueueFamilyIndex = -1;								// graphcs队列族索引
	int presentQueueFamilyIndex = -1;								// 支持present的队列族索引
	for (const auto& queueFamily : queueFamilies)
	{
		static int i = 0;
		string supportFlag;
		auto flasg = queueFamily.queueFlags;		// 队列组类型
		if (flasg & VK_QUEUE_GRAPHICS_BIT)
		{
			//vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, )
			graphcsQueueFamilyIndex = i;
			supportFlag += " GRAPHICS";
		}
		if (flasg & VK_QUEUE_COMPUTE_BIT)
		{
			supportFlag += " COMPUTE";
		}
		if (flasg & VK_QUEUE_TRANSFER_BIT)
		{
			supportFlag += " TRANSFER";
		}
		if (flasg & VK_QUEUE_SPARSE_BINDING_BIT)
		{
			supportFlag += " SPARSE_BINDING";
		}
		if (flasg & VK_QUEUE_PROTECTED_BIT)
		{
			supportFlag += " PROTECTED";
		}
		logI("The queue-family index: %d\n", i);
		logI("support:%s\n", supportFlag.c_str());

		logI("queues:%u\n", queueFamily.queueCount);		// 队列组支持的队列数量

		if (queueFamily.timestampValidBits != 0)								// 队列是否支持时间戳
		{
			logI("support timestamp.\n");
		}
		else
		{
			logI("don't support timestamp.\n");
		}

		// 判断是否支持present
		if (presentQueueFamilyIndex == -1)
		{
			VkBool32 presentSupport = false;
			CHECK_VK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport));
			if (presentSupport)
			{
				presentQueueFamilyIndex = i;
				logI("The queue-family that support present is %d\n", presentQueueFamilyIndex);
			}
		}

		i++;
	}

	vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	float queuePriority = 1.0f;														// 队列执行优先级

	// 指定要创建的graphics队列，用于创建逻辑设备对象
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.queueFamilyIndex = graphcsQueueFamilyIndex;
	graphicsQueueCreateInfo.queueCount = 1;											// 要创建的队列数量
	graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;						// 队列中指令执行的优先级，在0-1之间
	queueCreateInfos.push_back(graphicsQueueCreateInfo);

	// 如果present和grahpics队列族相同，则不额外创建，否则额外再来一个
	if (graphcsQueueFamilyIndex != presentQueueFamilyIndex) {
		// 指定要创建的present队列，用于创建逻辑设备对象
		VkDeviceQueueCreateInfo presentQueueCreateInfo = {};
		presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		presentQueueCreateInfo.queueFamilyIndex = presentQueueFamilyIndex;
		presentQueueCreateInfo.queueCount = 1;											// 要创建的队列数量
		presentQueueCreateInfo.pQueuePriorities = &queuePriority;						// 队列中指令执行的优先级，在0-1之间
		queueCreateInfos.push_back(presentQueueCreateInfo);
	}

	// 指定使用设备的特性，用于创建逻辑设备对象
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// 查看设备支持的扩展
	uint32_t deviceExtCount;
	CHECK_VK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtCount, nullptr));
	vector<VkExtensionProperties> deviceExt(deviceExtCount);
	CHECK_VK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtCount, deviceExt.data()));
	cout << "Device supported extensions:" << endl;
	for (auto& ext : deviceExt)
	{
		cout << ext.extensionName << endl;
	}
	// 启用交换链扩展
	vector<const char*> usedDeviceExt;
	usedDeviceExt.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// 创建逻辑设备
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = usedDeviceExt.size();
	deviceCreateInfo.ppEnabledExtensionNames = usedDeviceExt.data();
	deviceCreateInfo.enabledLayerCount = usedLayers.size();
	deviceCreateInfo.ppEnabledLayerNames = usedLayers.data();

	// 创建逻辑设备对象
	VkDevice vkDevice;
	CHECK_VK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &vkDevice));

	// 获取graphcs和present队列句柄
	VkQueue graphicsQueue, presentQueue;
	vkGetDeviceQueue(vkDevice, graphcsQueueFamilyIndex, 0, &graphicsQueue);		// 参数：逻辑设备，队列组索引，队列索引，返回队列句柄地址
	vkGetDeviceQueue(vkDevice, presentQueueFamilyIndex, 0, &presentQueue);		// 参数：逻辑设备，队列组索引，队列索引，返回队列句柄地址

	// 查询交换链基础表面属性
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	CHECK_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
	logI("min image cout: %d\n", surfaceCapabilities.minImageCount);
	logI("max image cout: %d\n", surfaceCapabilities.maxImageCount);
	logI("current surface width:%d height:%d\n", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
	logI("current surface min image width:%d height:%d\n", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height);
	logI("current surface max image width:%d height:%d\n", surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
	logI("max image array layers:%d\n", surfaceCapabilities.maxImageArrayLayers);

	// 查询交换链支持的表面格式
	uint32_t surfaceFormatCount;
	CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
	vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));

	// 查询交换链支持的呈现方式
	uint32_t presentModeCount;
	CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
	vector<VkPresentModeKHR> presentModes(presentModeCount);
	CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

	// 描述表面宽高
	VkExtent2D extent{ windowWidth, windowHeight };

	// 创建交换链
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;			// 使用三缓冲
	swapchainCreateInfo.imageFormat = surfaceFormats[0].format;
	swapchainCreateInfo.imageColorSpace = surfaceFormats[0].colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;				// 描述表面用途，此处用于附着到颜色缓冲，若要对图像进行后处理，可使用VK_IAMGE_SUAGE_TRANSFER_DST_BIT
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;					// 控制多个队列对图像的访问方式
	swapchainCreateInfo.queueFamilyIndexCount = 0;										// 有表面共享权的队列族数量
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;									// 有表面共享权索引数组
	swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;			// 指定一个固定的变换操作，比如旋转、翻转等，需要交换链有supportedTransforms特性
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;				// 指定alpha通道是否用来和窗口系统中其它窗口进行混合操作，此时忽略alpha通道
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;						// 使用三缓冲
	swapchainCreateInfo.clipped = VK_TRUE;												// 表示不关系被窗口系统中其它窗口遮挡的像素颜色，这允许Vulkan采取一定的优化措施，但回读窗口像素值可能出问题
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;									// 和窗口改变与交换链重建有关
	VkSwapchainKHR swapChain;
	CHECK_VK(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, nullptr, &swapChain));

	// 获得交换链图像
	uint32_t spImageCount;
	CHECK_VK(vkGetSwapchainImagesKHR(vkDevice, swapChain, &spImageCount, nullptr));
	vector<VkImage> swapchainImages(spImageCount);
	CHECK_VK(vkGetSwapchainImagesKHR(vkDevice, swapChain, &spImageCount, swapchainImages.data()));

	// 创建交换链图像视图
	vector<VkImageView> spImageViews(spImageCount);
	for (uint32_t i = 0; i < spImageCount; i++)
	{
		VkImageViewCreateInfo spImagesViewCreateInfo = {};
		spImagesViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		spImagesViewCreateInfo.image = swapchainImages[i];
		spImagesViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;						// 2维纹理
		spImagesViewCreateInfo.format = surfaceFormats[0].format;
		// 以下四个都指定颜色通道的映射，均采用默认映射
		spImagesViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		spImagesViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		spImagesViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		spImagesViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		// 指定图像用途和图像哪一部分可被访问，此处图像用于渲染目标，没有细分级别，只有一个图层
		spImagesViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		spImagesViewCreateInfo.subresourceRange.baseMipLevel = 0;
		spImagesViewCreateInfo.subresourceRange.levelCount = 1;
		spImagesViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		spImagesViewCreateInfo.subresourceRange.layerCount = 1;
		CHECK_VK(vkCreateImageView(vkDevice, &spImagesViewCreateInfo, nullptr, &spImageViews[i]));
	}

	// 渲染准备
	logI("Start to prepare draw---------------------------------------\n");

	// 创建vs和fs
	auto vsCode = readShaderFile("D:\\code\\Vulkan_learn\\Vulkan_learn\\vert.spv");
	auto fsCode = readShaderFile("D:\\code\\Vulkan_learn\\Vulkan_learn\\frag.spv");
	logI("vsCode size: %d\n", vsCode.size());
	logI("fsCode size: %d\n", fsCode.size());
	auto vsShaderModule = createShaderModule(vkDevice, vsCode);
	auto fsShaderModule = createShaderModule(vkDevice, fsCode);

	VkPipelineShaderStageCreateInfo vertShaderStateInfo = {};
	vertShaderStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStateInfo.module = vsShaderModule;
	vertShaderStateInfo.pName = "main";									// 可在一份代码中实现多个着色器，通过不同pName调用它们
	vertShaderStateInfo.pSpecializationInfo = nullptr;					// 用于指定着色器用到的常量，使得编译器可根据着色器常量消除一些条件分支

	VkPipelineShaderStageCreateInfo fragShaderStateInfo = {};
	fragShaderStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStateInfo.module = fsShaderModule;
	fragShaderStateInfo.pName = "main";									// 可在一份代码中实现多个着色器，通过不同pName调用它们
	fragShaderStateInfo.pSpecializationInfo = nullptr;					// 用于指定着色器用到的常量，使得编译器可根据着色器常量消除一些条件分支

	// 着色器阶段
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStateInfo, fragShaderStateInfo };

	// 描述顶点数据
	VkPipelineVertexInputStateCreateInfo vtxDescCreateInfo = {};
	vtxDescCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vtxDescCreateInfo.vertexBindingDescriptionCount = 0;
	vtxDescCreateInfo.vertexAttributeDescriptionCount = 0;

	// 定义输入装配
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;					// TRUE的话带_STRIP结尾的图元，可通过特殊索引值0XFFFF或0XFFFFFFFF重启图元（从特殊索引值后的索引重置为图元的第一个顶点）

	// 设置视口和裁剪区域
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// 设置光栅化
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1;									//指定线段宽度，线宽最大值依赖于硬件，大于1的线宽需要启用GPU相应特性
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;				// 指定剔除表面
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;				// 指定图元绕序
	rasterizer.depthBiasEnable = VK_FALSE;						// 接下来都是和偏移有关的
	rasterizer.depthBiasConstantFactor = 0;
	rasterizer.depthBiasClamp = 0;
	rasterizer.depthBiasSlopeFactor = 0;

	// 设置多重采样，依赖GPU特性
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//设置深度和模板

	// 设置混合
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0;
	colorBlending.blendConstants[1] = 0;
	colorBlending.blendConstants[2] = 0;
	colorBlending.blendConstants[3] = 0;

	// 指定需要修改的状态
	//VkDynamicState dynamicStates[] =
	//{
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	//dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//dynamicState.dynamicStateCount = 2;
	//dynamicState.pDynamicStates = dynamicStates;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates = nullptr;

	// 指定管线布局（uniform变量）
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	VkPipelineLayout pipelineLayout;
	CHECK_VK(vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));

	// 描述颜色附着信息
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = surfaceFormats[0].format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;					// 指定采样数
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				// 指定渲染前图像布局方式，此处不关心
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;			// 指定渲染后图像布局方式，用于被交换链呈现
	//colorAttachment.stencilLoadOp;		// 暂时不关心
	//colorAttachment.stencilStoreOp;

	// 指定使用附着
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;												// 指定要引用的附着在附着描述结构体中的索引
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			// 指定引用附着的布局方式

	// 描述子流程
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pInputAttachments;
	subpass.pResolveAttachments;
	subpass.pDepthStencilAttachment;

	// 配置子流程依赖
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;				// 指定被依赖的子流程索引，此处设置为隐含的子流程（表示渲染流程开始前的子流程）
	dependency.dstSubpass = 0;									// 依赖的子流程索引，0是之前创建的子流程的索引。为避免循环依赖，dstSubPass设置的值必须始终大于srcSubpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// 指定需要等待的管线阶段，此处等待交换链结束
	dependency.srcAccessMask = 0;												// 指定子流程即将执行的操作类型
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;								// 指定需要等待的管线阶段，设置为等待颜色附着的输出
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// 指定子流程即将进行的操作，设置为对颜色附着读写

	// 创建渲染流程
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	VkRenderPass renderPass;
	CHECK_VK(vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &renderPass));

	// 创建流水线对象
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vtxDescCreateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;							// 接下来两个变量用于是否在一个已创建好的管线基础上创建一个新管线，这么做创建的代价小，切换效率高
	pipelineInfo.basePipelineIndex = -1;										// 目前不用，所以这么设，两个变量只在VkGraphicsPipelineCreateInfo::flags使用VK_PIPELINE_CREATE_DERIVATIVE_BIT才起效
	VkPipeline graphicsPipeline;
	CHECK_VK(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline));

	// 为交换链视图创建帧缓冲
	vector<VkFramebuffer> spFramebuffer(spImageViews.size());
	for (uint32_t i = 0; i < spImageViews.size(); i++)
	{
		VkImageView attachments[] = { spImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;
		CHECK_VK(vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &spFramebuffer[i]));
	}

// 绘制

	// 创建指令池
	VkCommandPoolCreateInfo poolcreateInfo = {};
	poolcreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolcreateInfo.queueFamilyIndex = graphcsQueueFamilyIndex;
	poolcreateInfo.flags = 0;
	VkCommandPool cmdPool;
	CHECK_VK(vkCreateCommandPool(vkDevice, &poolcreateInfo, nullptr, &cmdPool));

	// 分配指令缓冲（为交换链中每个图像）
	vector<VkCommandBuffer> cmdBuffers(spFramebuffer.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = cmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)cmdBuffers.size();
	CHECK_VK(vkAllocateCommandBuffers(vkDevice, &allocInfo, cmdBuffers.data()));

	// 渲染流程
	for (uint32_t i = 0; i < cmdBuffers.size(); i++)
	{
		// 开始记录指令
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;			// 指定怎样使用指令缓冲
		beginInfo.pInheritanceInfo = nullptr;									// 用于辅助指令缓冲，用来指定从调用它的主要指令缓冲继承的状态
		CHECK_VK(vkBeginCommandBuffer(cmdBuffers[i], &beginInfo));				// 调用后会重置指令缓冲对象

		// 开始一个渲染流程
		VkClearValue clearColor = { 0, 0, 0, 1 };
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = spFramebuffer[i];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };						// renderArea用于指定渲染区域
		renderPassBeginInfo.renderArea.extent = extent;
		renderPassBeginInfo.clearValueCount = 1;								// clearValueCount和pClearValues用于指定使用VK_ATTACHMENT_LOAD_OP_CLEAR标记
		renderPassBeginInfo.pClearValues = &clearColor;							// 后，使用的清除值
		vkCmdBeginRenderPass(cmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);		// 最后一个参数用来指定渲染流程如何提供绘制指令的标记，此处表示所有指令都要在主要指令缓冲中，没有辅助指令缓冲需要执行
		vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(cmdBuffers[i], 3, 1, 0, 0);		// [p2：顶点数] [p3:：1表示不进行实例渲染] [p4：用于定义实例索引gl_InstanceIndex的值]

		// 结束渲染流程
		vkCmdEndRenderPass(cmdBuffers[i]);

		// 记录指令到指令缓冲
		CHECK_VK(vkEndCommandBuffer(cmdBuffers[i]));
	}

	// 创建信号量
	VkSemaphore imgAvaSem;										// 表示图像已被获取，可开始渲染
	VkSemaphore renderFinSem;									// 发出渲染已经结束，可开始呈现
	VkSemaphoreCreateInfo semCreateInfo = {};
	semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	CHECK_VK(vkCreateSemaphore(vkDevice, &semCreateInfo, nullptr, &imgAvaSem));
	CHECK_VK(vkCreateSemaphore(vkDevice, &semCreateInfo, nullptr, &renderFinSem));

	int maxFrames = 100;
	int curFrames = 0;
	while (1)
	{
		curFrames++;

		// 从交换链获取图像
		uint32_t imgIndex;
		CHECK_VK(vkAcquireNextImageKHR(vkDevice, swapChain, UINT64_MAX, imgAvaSem, VK_NULL_HANDLE, &imgIndex));

		logI("imgIndex: %d\n", imgIndex);

		// 向命令队列提指令缓冲
		VkSemaphore waitSemaphores[] = { imgAvaSem };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinSem };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;									// 队列开始执行时需要等待的信号量数量
		submitInfo.pWaitSemaphores = waitSemaphores;						// 队列开始执行时需要等待的信号量数组
		submitInfo.pWaitDstStageMask = waitStages;							// 需要等待的管线阶段，条目和pWaitSemaphores相对应
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffers[imgIndex];					// 指定指令被提交到的缓冲对象
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;					// 用于指定指令缓冲执行结束后发出信号的信号量对象
		CHECK_VK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));		// 可提交多个指令指令缓冲，最后一个参数制定一个可选的栅栏，用于同步指令执行结束后的操作

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;						// 开始呈现时要等待的信号量
		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imgIndex;
		presentInfo.pResults = nullptr;							// 用于获取交换链呈现操作是否成功的信息
		CHECK_VK(vkQueuePresentKHR(presentQueue, &presentInfo));

		CHECK_VK(vkDeviceWaitIdle(vkDevice));					// 等待逻辑设备操作结束
	}


	// 结束部分
	logI("Start to end the program-----------------------------------------\n");

	CHECK_VK(vkDeviceWaitIdle(vkDevice));

	// 清除信号量
	vkDestroySemaphore(vkDevice, imgAvaSem, nullptr);
	vkDestroySemaphore(vkDevice, renderFinSem, nullptr);

	// 删除指令池
	vkDestroyCommandPool(vkDevice, cmdPool, nullptr);

	// 清除帧缓冲
	for (auto framebuffer : spFramebuffer)
	{
		vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
	}

	// 删除流水线
	vkDestroyPipeline(vkDevice, graphicsPipeline, nullptr);

	// 删除渲染流程
	vkDestroyRenderPass(vkDevice, renderPass, nullptr);

	// 删除管线布局
	vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);

	// 删除vs和fs
	vkDestroyShaderModule(vkDevice, vsShaderModule, nullptr);
	vkDestroyShaderModule(vkDevice, fsShaderModule, nullptr);

	// 删除交换链图像视图
	for (uint32_t i = 0; i < spImageCount; i++)
	{
		vkDestroyImageView(vkDevice, spImageViews[i], nullptr);
	}


	// 销毁交换链
	vkDestroySwapchainKHR(vkDevice, swapChain, nullptr);

	// 销毁表面
	vkDestroySurfaceKHR(vkInstance, surface, nullptr);

	// 删除逻辑设备对象
	vkDestroyDevice(vkDevice, nullptr);


#ifdef OPEN_DEBUG
	// 注销DEBUG回调
	if (!setdownDebugExt(vkInstance, dbgUtilMsgExt))
	{
		logE("Failed to setdownDebugExt\n");
	}
#endif

	// 释放Vulkan实例
	vkDestroyInstance(vkInstance, nullptr);

	// GLFW结束
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();

	return 0;
}

#endif