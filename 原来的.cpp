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

// ����VK_EXT_debug_tuilsУ�����չ�Ļص�����
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
	VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,				// ָ������Ϣ����
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

// ��������
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

// ����VK_EXT_debug_tuilsУ�����չ��ע��ص�����
bool setupDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT* callback)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =														// ָ���ص������Ĵ�����Ϣ����
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =															// ָ���ص������������Ϣ����
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallBack;										// ע��ص�����
	createInfo.pUserData = nullptr;													// ��ص����������û��Լ����������

	// ����vkCreateDebugUtilsMessengerEXT����
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func == nullptr)
	{
		logE("Failed to get function named vkCreateDebugUtilsMessengerEXT\n");
		return false;
	}

	// ע��ص�����
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

// �ж�һ���Կ��Ƿ�����Ҫ��Ŀǰûɶ�ã��������㣩
bool isDeviceSuitable(VkPhysicalDevice device)
{
	// ����豸����
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	logI("Device name: %s\n", deviceProperties.deviceName);
	logI("Device driver version: %u\n", deviceProperties.driverVersion);

	// ����豸֧������
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

	// GLFW��ʼ������
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "window", nullptr, nullptr);

	// Vulkan��ʼ������
		// VkApplicationInfo���ڸ�֪����ʹ����ʲô���桢��ʲô�����
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

	// ����ṹ�壬���ڸ���vk����Ҫ�õ�ȫ����չ��У������Ϣ
	// ��ʹ���κ���չ����ʹ���κ�У���
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledExtensionNames = nullptr;
	createInfo.enabledLayerCount = 0;

	// ��ʼ��Vulkanʵ����ָ����һЩ����������Ҫʹ�õ�Ӧ�ó�����Ϣ
	CHECK_VK(vkCreateInstance(&createInfo, nullptr, &vkInstance));
#else
	// ���֧�ֵ���չ
	uint32_t extensionCout = 0;
	CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCout, nullptr));
	vector<VkExtensionProperties> extensions(extensionCout);
	CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCout, extensions.data()));
	cout << "Extensions: " << endl;
	for (const auto& extension : extensions)
	{
		cout << extension.extensionName << endl;
	}

	// ���֧�ֵ�У���
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

	// ����ָ����չ
	vector<const char*> usedExt;
	// usedExt.push_back("VK_EXT_debug_report");
	usedExt.push_back("VK_EXT_debug_utils");
	usedExt.push_back("VK_KHR_win32_surface");
	usedExt.push_back("VK_KHR_surface");

	// ����ṹ�壬���ڸ���vk����Ҫ�õ�ȫ����չ��У������Ϣ
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = usedExt.size();
	createInfo.ppEnabledExtensionNames = usedExt.data();
	createInfo.enabledLayerCount = usedLayers.size();
	createInfo.ppEnabledLayerNames = usedLayers.data();

	// ��ʼ��Vulkanʵ����ָ����һЩ����������Ҫʹ�õ�Ӧ�ó�����Ϣ
	CHECK_VK(vkCreateInstance(&createInfo, nullptr, &vkInstance));

	// ע��DEBUG�ص�
	VkDebugUtilsMessengerEXT dbgUtilMsgExt;
	if (!setupDebugExt(vkInstance, &dbgUtilMsgExt))
	{
		logE("Failed to setupDebugExt\n");
	}
#endif

	// �������ڱ���
	VkSurfaceKHR surface;
	CHECK_VK(glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &surface));

	// ѡ�������豸
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

	// ѡ��GRAPHICS������
	uint32_t queueFamliyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamliyCount, nullptr);
	vector<VkQueueFamilyProperties> queueFamilies(queueFamliyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamliyCount, queueFamilies.data());
	int graphcsQueueFamilyIndex = -1;								// graphcs����������
	int presentQueueFamilyIndex = -1;								// ֧��present�Ķ���������
	for (const auto& queueFamily : queueFamilies)
	{
		static int i = 0;
		string supportFlag;
		auto flasg = queueFamily.queueFlags;		// ����������
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

		logI("queues:%u\n", queueFamily.queueCount);		// ������֧�ֵĶ�������

		if (queueFamily.timestampValidBits != 0)								// �����Ƿ�֧��ʱ���
		{
			logI("support timestamp.\n");
		}
		else
		{
			logI("don't support timestamp.\n");
		}

		// �ж��Ƿ�֧��present
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

	float queuePriority = 1.0f;														// ����ִ�����ȼ�

	// ָ��Ҫ������graphics���У����ڴ����߼��豸����
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.queueFamilyIndex = graphcsQueueFamilyIndex;
	graphicsQueueCreateInfo.queueCount = 1;											// Ҫ�����Ķ�������
	graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;						// ������ָ��ִ�е����ȼ�����0-1֮��
	queueCreateInfos.push_back(graphicsQueueCreateInfo);

	// ���present��grahpics��������ͬ���򲻶��ⴴ���������������һ��
	if (graphcsQueueFamilyIndex != presentQueueFamilyIndex) {
		// ָ��Ҫ������present���У����ڴ����߼��豸����
		VkDeviceQueueCreateInfo presentQueueCreateInfo = {};
		presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		presentQueueCreateInfo.queueFamilyIndex = presentQueueFamilyIndex;
		presentQueueCreateInfo.queueCount = 1;											// Ҫ�����Ķ�������
		presentQueueCreateInfo.pQueuePriorities = &queuePriority;						// ������ָ��ִ�е����ȼ�����0-1֮��
		queueCreateInfos.push_back(presentQueueCreateInfo);
	}

	// ָ��ʹ���豸�����ԣ����ڴ����߼��豸����
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// �鿴�豸֧�ֵ���չ
	uint32_t deviceExtCount;
	CHECK_VK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtCount, nullptr));
	vector<VkExtensionProperties> deviceExt(deviceExtCount);
	CHECK_VK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtCount, deviceExt.data()));
	cout << "Device supported extensions:" << endl;
	for (auto& ext : deviceExt)
	{
		cout << ext.extensionName << endl;
	}
	// ���ý�������չ
	vector<const char*> usedDeviceExt;
	usedDeviceExt.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// �����߼��豸
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = usedDeviceExt.size();
	deviceCreateInfo.ppEnabledExtensionNames = usedDeviceExt.data();
	deviceCreateInfo.enabledLayerCount = usedLayers.size();
	deviceCreateInfo.ppEnabledLayerNames = usedLayers.data();

	// �����߼��豸����
	VkDevice vkDevice;
	CHECK_VK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &vkDevice));

	// ��ȡgraphcs��present���о��
	VkQueue graphicsQueue, presentQueue;
	vkGetDeviceQueue(vkDevice, graphcsQueueFamilyIndex, 0, &graphicsQueue);		// �������߼��豸���������������������������ض��о����ַ
	vkGetDeviceQueue(vkDevice, presentQueueFamilyIndex, 0, &presentQueue);		// �������߼��豸���������������������������ض��о����ַ

	// ��ѯ������������������
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	CHECK_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
	logI("min image cout: %d\n", surfaceCapabilities.minImageCount);
	logI("max image cout: %d\n", surfaceCapabilities.maxImageCount);
	logI("current surface width:%d height:%d\n", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
	logI("current surface min image width:%d height:%d\n", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height);
	logI("current surface max image width:%d height:%d\n", surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
	logI("max image array layers:%d\n", surfaceCapabilities.maxImageArrayLayers);

	// ��ѯ������֧�ֵı����ʽ
	uint32_t surfaceFormatCount;
	CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
	vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));

	// ��ѯ������֧�ֵĳ��ַ�ʽ
	uint32_t presentModeCount;
	CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
	vector<VkPresentModeKHR> presentModes(presentModeCount);
	CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

	// ����������
	VkExtent2D extent{ windowWidth, windowHeight };

	// ����������
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;			// ʹ��������
	swapchainCreateInfo.imageFormat = surfaceFormats[0].format;
	swapchainCreateInfo.imageColorSpace = surfaceFormats[0].colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;				// ����������;���˴����ڸ��ŵ���ɫ���壬��Ҫ��ͼ����к�����ʹ��VK_IAMGE_SUAGE_TRANSFER_DST_BIT
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;					// ���ƶ�����ж�ͼ��ķ��ʷ�ʽ
	swapchainCreateInfo.queueFamilyIndexCount = 0;										// �б��湲��Ȩ�Ķ���������
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;									// �б��湲��Ȩ��������
	swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;			// ָ��һ���̶��ı任������������ת����ת�ȣ���Ҫ��������supportedTransforms����
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;				// ָ��alphaͨ���Ƿ������ʹ���ϵͳ���������ڽ��л�ϲ�������ʱ����alphaͨ��
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;						// ʹ��������
	swapchainCreateInfo.clipped = VK_TRUE;												// ��ʾ����ϵ������ϵͳ�����������ڵ���������ɫ��������Vulkan��ȡһ�����Ż���ʩ�����ض���������ֵ���ܳ�����
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;									// �ʹ��ڸı��뽻�����ؽ��й�
	VkSwapchainKHR swapChain;
	CHECK_VK(vkCreateSwapchainKHR(vkDevice, &swapchainCreateInfo, nullptr, &swapChain));

	// ��ý�����ͼ��
	uint32_t spImageCount;
	CHECK_VK(vkGetSwapchainImagesKHR(vkDevice, swapChain, &spImageCount, nullptr));
	vector<VkImage> swapchainImages(spImageCount);
	CHECK_VK(vkGetSwapchainImagesKHR(vkDevice, swapChain, &spImageCount, swapchainImages.data()));

	// ����������ͼ����ͼ
	vector<VkImageView> spImageViews(spImageCount);
	for (uint32_t i = 0; i < spImageCount; i++)
	{
		VkImageViewCreateInfo spImagesViewCreateInfo = {};
		spImagesViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		spImagesViewCreateInfo.image = swapchainImages[i];
		spImagesViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;						// 2ά����
		spImagesViewCreateInfo.format = surfaceFormats[0].format;
		// �����ĸ���ָ����ɫͨ����ӳ�䣬������Ĭ��ӳ��
		spImagesViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		spImagesViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		spImagesViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		spImagesViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		// ָ��ͼ����;��ͼ����һ���ֿɱ����ʣ��˴�ͼ��������ȾĿ�꣬û��ϸ�ּ���ֻ��һ��ͼ��
		spImagesViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		spImagesViewCreateInfo.subresourceRange.baseMipLevel = 0;
		spImagesViewCreateInfo.subresourceRange.levelCount = 1;
		spImagesViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		spImagesViewCreateInfo.subresourceRange.layerCount = 1;
		CHECK_VK(vkCreateImageView(vkDevice, &spImagesViewCreateInfo, nullptr, &spImageViews[i]));
	}

	// ��Ⱦ׼��
	logI("Start to prepare draw---------------------------------------\n");

	// ����vs��fs
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
	vertShaderStateInfo.pName = "main";									// ����һ�ݴ�����ʵ�ֶ����ɫ����ͨ����ͬpName��������
	vertShaderStateInfo.pSpecializationInfo = nullptr;					// ����ָ����ɫ���õ��ĳ�����ʹ�ñ������ɸ�����ɫ����������һЩ������֧

	VkPipelineShaderStageCreateInfo fragShaderStateInfo = {};
	fragShaderStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStateInfo.module = fsShaderModule;
	fragShaderStateInfo.pName = "main";									// ����һ�ݴ�����ʵ�ֶ����ɫ����ͨ����ͬpName��������
	fragShaderStateInfo.pSpecializationInfo = nullptr;					// ����ָ����ɫ���õ��ĳ�����ʹ�ñ������ɸ�����ɫ����������һЩ������֧

	// ��ɫ���׶�
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStateInfo, fragShaderStateInfo };

	// ������������
	VkPipelineVertexInputStateCreateInfo vtxDescCreateInfo = {};
	vtxDescCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vtxDescCreateInfo.vertexBindingDescriptionCount = 0;
	vtxDescCreateInfo.vertexAttributeDescriptionCount = 0;

	// ��������װ��
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;					// TRUE�Ļ���_STRIP��β��ͼԪ����ͨ����������ֵ0XFFFF��0XFFFFFFFF����ͼԪ������������ֵ�����������ΪͼԪ�ĵ�һ�����㣩

	// �����ӿںͲü�����
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

	// ���ù�դ��
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1;									//ָ���߶ο�ȣ��߿����ֵ������Ӳ��������1���߿���Ҫ����GPU��Ӧ����
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;				// ָ���޳�����
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;				// ָ��ͼԪ����
	rasterizer.depthBiasEnable = VK_FALSE;						// ���������Ǻ�ƫ���йص�
	rasterizer.depthBiasConstantFactor = 0;
	rasterizer.depthBiasClamp = 0;
	rasterizer.depthBiasSlopeFactor = 0;

	// ���ö��ز���������GPU����
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//������Ⱥ�ģ��

	// ���û��
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

	// ָ����Ҫ�޸ĵ�״̬
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

	// ָ�����߲��֣�uniform������
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	VkPipelineLayout pipelineLayout;
	CHECK_VK(vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));

	// ������ɫ������Ϣ
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = surfaceFormats[0].format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;					// ָ��������
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				// ָ����Ⱦǰͼ�񲼾ַ�ʽ���˴�������
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;			// ָ����Ⱦ��ͼ�񲼾ַ�ʽ�����ڱ�����������
	//colorAttachment.stencilLoadOp;		// ��ʱ������
	//colorAttachment.stencilStoreOp;

	// ָ��ʹ�ø���
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;												// ָ��Ҫ���õĸ����ڸ��������ṹ���е�����
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			// ָ�����ø��ŵĲ��ַ�ʽ

	// ����������
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pInputAttachments;
	subpass.pResolveAttachments;
	subpass.pDepthStencilAttachment;

	// ��������������
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;				// ָ�����������������������˴�����Ϊ�����������̣���ʾ��Ⱦ���̿�ʼǰ�������̣�
	dependency.dstSubpass = 0;									// ������������������0��֮ǰ�����������̵�������Ϊ����ѭ��������dstSubPass���õ�ֵ����ʼ�մ���srcSubpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// ָ����Ҫ�ȴ��Ĺ��߽׶Σ��˴��ȴ�����������
	dependency.srcAccessMask = 0;												// ָ�������̼���ִ�еĲ�������
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;								// ָ����Ҫ�ȴ��Ĺ��߽׶Σ�����Ϊ�ȴ���ɫ���ŵ����
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// ָ�������̼������еĲ���������Ϊ����ɫ���Ŷ�д

	// ������Ⱦ����
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

	// ������ˮ�߶���
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
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;							// �������������������Ƿ���һ���Ѵ����õĹ��߻����ϴ���һ���¹��ߣ���ô�������Ĵ���С���л�Ч�ʸ�
	pipelineInfo.basePipelineIndex = -1;										// Ŀǰ���ã�������ô�裬��������ֻ��VkGraphicsPipelineCreateInfo::flagsʹ��VK_PIPELINE_CREATE_DERIVATIVE_BIT����Ч
	VkPipeline graphicsPipeline;
	CHECK_VK(vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline));

	// Ϊ��������ͼ����֡����
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

// ����

	// ����ָ���
	VkCommandPoolCreateInfo poolcreateInfo = {};
	poolcreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolcreateInfo.queueFamilyIndex = graphcsQueueFamilyIndex;
	poolcreateInfo.flags = 0;
	VkCommandPool cmdPool;
	CHECK_VK(vkCreateCommandPool(vkDevice, &poolcreateInfo, nullptr, &cmdPool));

	// ����ָ��壨Ϊ��������ÿ��ͼ��
	vector<VkCommandBuffer> cmdBuffers(spFramebuffer.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = cmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)cmdBuffers.size();
	CHECK_VK(vkAllocateCommandBuffers(vkDevice, &allocInfo, cmdBuffers.data()));

	// ��Ⱦ����
	for (uint32_t i = 0; i < cmdBuffers.size(); i++)
	{
		// ��ʼ��¼ָ��
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;			// ָ������ʹ��ָ���
		beginInfo.pInheritanceInfo = nullptr;									// ���ڸ���ָ��壬����ָ���ӵ���������Ҫָ���̳е�״̬
		CHECK_VK(vkBeginCommandBuffer(cmdBuffers[i], &beginInfo));				// ���ú������ָ������

		// ��ʼһ����Ⱦ����
		VkClearValue clearColor = { 0, 0, 0, 1 };
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = spFramebuffer[i];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };						// renderArea����ָ����Ⱦ����
		renderPassBeginInfo.renderArea.extent = extent;
		renderPassBeginInfo.clearValueCount = 1;								// clearValueCount��pClearValues����ָ��ʹ��VK_ATTACHMENT_LOAD_OP_CLEAR���
		renderPassBeginInfo.pClearValues = &clearColor;							// ��ʹ�õ����ֵ
		vkCmdBeginRenderPass(cmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);		// ���һ����������ָ����Ⱦ��������ṩ����ָ��ı�ǣ��˴���ʾ����ָ�Ҫ����Ҫָ����У�û�и���ָ�����Ҫִ��
		vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(cmdBuffers[i], 3, 1, 0, 0);		// [p2��������] [p3:��1��ʾ������ʵ����Ⱦ] [p4�����ڶ���ʵ������gl_InstanceIndex��ֵ]

		// ������Ⱦ����
		vkCmdEndRenderPass(cmdBuffers[i]);

		// ��¼ָ�ָ���
		CHECK_VK(vkEndCommandBuffer(cmdBuffers[i]));
	}

	// �����ź���
	VkSemaphore imgAvaSem;										// ��ʾͼ���ѱ���ȡ���ɿ�ʼ��Ⱦ
	VkSemaphore renderFinSem;									// ������Ⱦ�Ѿ��������ɿ�ʼ����
	VkSemaphoreCreateInfo semCreateInfo = {};
	semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	CHECK_VK(vkCreateSemaphore(vkDevice, &semCreateInfo, nullptr, &imgAvaSem));
	CHECK_VK(vkCreateSemaphore(vkDevice, &semCreateInfo, nullptr, &renderFinSem));

	int maxFrames = 100;
	int curFrames = 0;
	while (1)
	{
		curFrames++;

		// �ӽ�������ȡͼ��
		uint32_t imgIndex;
		CHECK_VK(vkAcquireNextImageKHR(vkDevice, swapChain, UINT64_MAX, imgAvaSem, VK_NULL_HANDLE, &imgIndex));

		logI("imgIndex: %d\n", imgIndex);

		// �����������ָ���
		VkSemaphore waitSemaphores[] = { imgAvaSem };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinSem };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;									// ���п�ʼִ��ʱ��Ҫ�ȴ����ź�������
		submitInfo.pWaitSemaphores = waitSemaphores;						// ���п�ʼִ��ʱ��Ҫ�ȴ����ź�������
		submitInfo.pWaitDstStageMask = waitStages;							// ��Ҫ�ȴ��Ĺ��߽׶Σ���Ŀ��pWaitSemaphores���Ӧ
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffers[imgIndex];					// ָ��ָ��ύ���Ļ������
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;					// ����ָ��ָ���ִ�н����󷢳��źŵ��ź�������
		CHECK_VK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));		// ���ύ���ָ��ָ��壬���һ�������ƶ�һ����ѡ��դ��������ͬ��ָ��ִ�н�����Ĳ���

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;						// ��ʼ����ʱҪ�ȴ����ź���
		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imgIndex;
		presentInfo.pResults = nullptr;							// ���ڻ�ȡ���������ֲ����Ƿ�ɹ�����Ϣ
		CHECK_VK(vkQueuePresentKHR(presentQueue, &presentInfo));

		CHECK_VK(vkDeviceWaitIdle(vkDevice));					// �ȴ��߼��豸��������
	}


	// ��������
	logI("Start to end the program-----------------------------------------\n");

	CHECK_VK(vkDeviceWaitIdle(vkDevice));

	// ����ź���
	vkDestroySemaphore(vkDevice, imgAvaSem, nullptr);
	vkDestroySemaphore(vkDevice, renderFinSem, nullptr);

	// ɾ��ָ���
	vkDestroyCommandPool(vkDevice, cmdPool, nullptr);

	// ���֡����
	for (auto framebuffer : spFramebuffer)
	{
		vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
	}

	// ɾ����ˮ��
	vkDestroyPipeline(vkDevice, graphicsPipeline, nullptr);

	// ɾ����Ⱦ����
	vkDestroyRenderPass(vkDevice, renderPass, nullptr);

	// ɾ�����߲���
	vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);

	// ɾ��vs��fs
	vkDestroyShaderModule(vkDevice, vsShaderModule, nullptr);
	vkDestroyShaderModule(vkDevice, fsShaderModule, nullptr);

	// ɾ��������ͼ����ͼ
	for (uint32_t i = 0; i < spImageCount; i++)
	{
		vkDestroyImageView(vkDevice, spImageViews[i], nullptr);
	}


	// ���ٽ�����
	vkDestroySwapchainKHR(vkDevice, swapChain, nullptr);

	// ���ٱ���
	vkDestroySurfaceKHR(vkInstance, surface, nullptr);

	// ɾ���߼��豸����
	vkDestroyDevice(vkDevice, nullptr);


#ifdef OPEN_DEBUG
	// ע��DEBUG�ص�
	if (!setdownDebugExt(vkInstance, dbgUtilMsgExt))
	{
		logE("Failed to setdownDebugExt\n");
	}
#endif

	// �ͷ�Vulkanʵ��
	vkDestroyInstance(vkInstance, nullptr);

	// GLFW����
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();

	return 0;
}

#endif