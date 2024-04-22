#include "VKBase.h"
#include <malloc.h>

#include <string.h>

using namespace std;

const char VK_DEBUG_UTILS[] = "VK_EXT_debug_utils";
const char VK_KHR_SURF[] = "VK_KHR_surface";
const char VK_LAYER_KHR_VAL[] = "VK_LAYER_KHRONOS_validation";

#ifndef NDEBUG

void checkErrCode(VkResult ret) {
#define VK_ERROR(...) logE("Vulkan Error:"##__VA_ARGS__);
	switch (ret) {
	case VK_ERROR_LAYER_NOT_PRESENT: {
		VK_ERROR("The layer is not present.");
		break;
}
	case VK_ERROR_EXTENSION_NOT_PRESENT: {
		VK_ERROR("Extension is not present.");
		break;
	}
	default: {
		VK_ERROR("Error code: %d", ret);
	}
	}
}

// ����VK_EXT_debug_tuilsУ�����չ�Ļص�����
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,				// ָ������Ϣ����
	VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* callBackData, void* userData) {
	string dbgStr("Severity[");
	switch (msgSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
		dbgStr += "VERBOSE]";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
		dbgStr += "INFO]";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
		dbgStr += "WARNING]";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
		dbgStr += "ERROR]";
		break;
	}
	default: {
		dbgStr += "UNKNOWN(Value:" + to_string(msgSeverity) + ")]";
	}
	}

	dbgStr += " Type[";
	switch (msgType) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
		dbgStr += "GENERAL]: ";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
		dbgStr += "VALIDATION]: ";
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
		dbgStr += "PERFORMANCE]";
		break;
	}
	default: {
		dbgStr += "UNKNOWN(" + to_string(msgType) + ")] ";
	}
	}
	dbgStr += "Message[";
	dbgStr += callBackData->pMessage;
	dbgStr += "]";
	logI("Vulkan debug callback info: %s\n", dbgStr.c_str());

	return VK_FALSE;
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
	CHECK_VK(func(instance, &createInfo, EALLOC_CB, callback));

	return true;
}

// ע���ص�����
bool setdownDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT callback)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func == nullptr) {
		logE("Failed to get function named vkDestroyDebugUtilsMessengerEXT\n");
		return false;
	}
	func(instance, callback, EALLOC_CB);
	return true;
}

#else

#endif

const char CommandTypeStr[][16] = { 
	{ "Graphics" },
	{ "Compute" },
	{ "TransferType" },
	{ "Protected" },
	{ "VideDeocde" },
	{ "VideEncode" },
	{ "OpticalFlow" }
};

const char deviceMemoryTypeStr[][16] = {
	{ "UploadHeap" },
	{ "DefaultHeap" },
	{ "ReadBackHeap" },
	{ "CustomHeap" }
};

// ��ȡָ��·�����ļ�
vector<char> readFile(const string& filepath) {
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

	if (buffer.size() == 0) {
		logE("Failed to read file(%s)\n", filepath.c_str());
		assert(0);
	}

	return buffer;
}

//// �ж�һ���Կ��Ƿ�����Ҫ��Ŀǰûɶ�ã��������㣩
//bool isDeviceSuitable(VkPhysicalDevice device)
//{
//	// ����豸����
//	VkPhysicalDeviceProperties deviceProperties;
//	vkGetPhysicalDeviceProperties(device, &deviceProperties);
//	uint32_t apiVersion = deviceProperties.apiVersion;
//	logI("Device name:[%s] Supported Vulkan version:[%d.%d.%d]\n", deviceProperties.deviceName, VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion), VK_API_VERSION_PATCH(apiVersion));
//
//	// ����豸֧������
//	VkPhysicalDeviceFeatures deviceFeatures;
//	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//
//	return true;
//}

class ImageSwapChain : Image {
public:

	virtual ~ImageSwapChain() {}

private:
	ImageSwapChain(VkImage img) : Image(img) {}

	friend class SwapChain;
};

Factory* Factory::myself = nullptr;
VkInstance Factory::instance = VK_NULL_HANDLE;
VkAllocationCallbacks* Factory::allocCB = nullptr;

Factory* Factory::createFactory(EngineInitInfo* info, VkAllocationCallbacks* allocCB) {
	if (myself != nullptr) {
		logE("%s: Failed to create Factory whose name is %s and which has been already created.\n", 
			__func__, myself->name.c_str());
		return myself;
	}

	myself = new Factory(info, allocCB);
	
	return myself;
}

Factory::Factory(EngineInitInfo* info, VkAllocationCallbacks* allocCB) {
	appName = info->appName;
	engineName = info->engineName;

	if (allocCB != nullptr) {
		auto cb = new VkAllocationCallbacks;
		*cb = *allocCB;
		allocCB = cb;
	}

	createVkInstance();
	getAllPhysicalDevice();
}

Factory::~Factory() {
	delInstance();

	delete allocCB;
	allocCB = nullptr;

	myself = nullptr;
}

bool Factory::getAdapterInfo(uint32_t phyDevIdx, VkPhysicalDeviceProperties* info) {
#if COMPILE_MODE > MODE_PERFORMANCE
	if (physicalDevices.size() == 0) {
		ElogE("The number of adapters is 0.\n");
		return false;
	}
	if (phyDevIdx >= physicalDevices.size()) {
		ElogE("Adapter index[%d] does not exist.\n", phyDevIdx);
		return false;
	}
	if (info == nullptr) {
		ElogE("Parameter \"info\" is nullptr.\n");
		return false;
	}
#endif

	vkGetPhysicalDeviceProperties(physicalDevices[phyDevIdx], info);

	return true;
}

bool Factory::getAdapterCapacities(uint32_t phyDevIdx, VkPhysicalDeviceFeatures* capacities) {
	if (physicalDevices.size() == 0) {
		ElogE("The number of adapters is 0.\n");
		return false;
	}
	if (phyDevIdx >= physicalDevices.size()) {
		ElogE("Adapter index[%d] does not exist.\n", phyDevIdx);
		return false;
	}
	if (capacities == nullptr) {
		ElogE("Parameter \"capacities\" is nullptr.\n");
		return false;
	}

	vkGetPhysicalDeviceFeatures(physicalDevices[phyDevIdx], capacities);

	return true;
}

Device* Factory::createDevice(uint32_t phyDevIdx = 0) {
	if (physicalDevices.size() == 0) {
		ElogE("The number of adapters is 0.\n");
		return nullptr;
	}
	if (phyDevIdx >= physicalDevices.size()) {
		ElogE("Adapter index[%d] does not exist.\n", phyDevIdx);
		return nullptr;
	}

	VkPhysicalDevice phyDev = physicalDevices[phyDevIdx];

	// ��������豸�Ķ�������Ϣ������ΪCommandObjectType�г�Ա���Ҷ�Ӧ�Ķ���������
	EVECTOR<VkQueueFamilyProperties> queueFamiliesProp;
	EVECTOR<int> cmdQueueFamilyIndexList;
	uint32_t maxFamilyQueueCount = getPhyDevCmdQueueTypeInfo(phyDev, queueFamiliesProp, cmdQueueFamilyIndexList);

	uint32_t queueFamilesCount = queueFamiliesProp.size();

	vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos(queueFamilesCount);	// ���ڴ���device�Ķ�����Ϣ

	vector<float> priorities(maxFamilyQueueCount, 1.f);
	for (uint32_t i = 0; i < queueFamilesCount; i++) {
		deviceQueueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfos[i].queueFamilyIndex = i;
		deviceQueueCreateInfos[i].queueCount = queueFamiliesProp[i].queueCount;
		deviceQueueCreateInfos[i].pQueuePriorities = priorities.data();					// ������ָ��ִ�е����ȼ�����0-1֮��
	}

	// �鿴�豸֧�ֵ���չ
	uint32_t deviceExtCount;
	CHECK_VK(vkEnumerateDeviceExtensionProperties(phyDev, nullptr, &deviceExtCount, nullptr));
	vector<VkExtensionProperties> deviceExt(deviceExtCount);
	CHECK_VK(vkEnumerateDeviceExtensionProperties(phyDev, nullptr, &deviceExtCount, deviceExt.data()));
#if COMPILE_MODE == MODE_DEBUG
	string extInfo("\nDevice supported extensions[");
	extInfo += (to_string(deviceExtCount) + "]:\n");
	for (uint32_t i = 0; i < deviceExtCount; i++) {
		extInfo += "Extension[";
		string idx = to_string(i);
		extInfo += (string(max(0, 3 - (int64_t)idx.length()), ' ') + idx + "]:[" + deviceExt[i].extensionName + "]\n");
	}
	ElogI("%s", extInfo.c_str());
#endif

	vector<const char*> usedDeviceExt;
	// ���ý�������չ
	usedDeviceExt.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// �����豸����
	VkPhysicalDeviceFeatures phyDevFeatures = {};

	// �����߼��豸
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = deviceQueueCreateInfos.size();
	deviceCreateInfo.pEnabledFeatures = &phyDevFeatures;
	deviceCreateInfo.enabledExtensionCount = usedDeviceExt.size();
	deviceCreateInfo.ppEnabledExtensionNames = usedDeviceExt.data();
	deviceCreateInfo.enabledLayerCount = usedLayers.size();
	deviceCreateInfo.ppEnabledLayerNames = usedLayers.data();
	
	// �����߼��豸����
	VkDevice device;
	CHECK_VK(vkCreateDevice(phyDev, &deviceCreateInfo, EALLOC_CB, &device));

	Device* ret = new Device(device, phyDev);

	// ָ������������������������
	ret->cmdQueueFamilyIndexList = std::move(cmdQueueFamilyIndexList);

	return ret;
}

void Factory::createVkInstance() {
	// ���API�汾��
	uint32_t apiVersion;
	if (vkEnumerateInstanceVersion != nullptr) {						// �˺�������1.00�汾���ṩ
		CHECK_VK(vkEnumerateInstanceVersion(&apiVersion));
		ElogI("VULKAN API VERSION:%d.%d.%d\n", VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion), VK_API_VERSION_PATCH(apiVersion));
	} else {
		apiVersion = VK_API_VERSION_1_0;
		ElogI("VULKAN API VERSION: 1.0.0\n");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = engineName.c_str();
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = apiVersion;

	// ���֧�ֵ���չ
	uint32_t extensionCout = 0;
	CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCout, nullptr));
	vector<VkExtensionProperties> extensions(extensionCout);
	CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCout, extensions.data()));
#if COMPILE_MODE == MODE_DEBUG
	string extInfo("\nInstance Supported Extensions[");
	extInfo += (to_string(extensionCout) + "]:\n");
	for (uint32_t i = 0; i < extensions.size(); i++) {
		extInfo += "Extension[";
		string idx = to_string(i);
		string specVer = to_string(extensions[i].specVersion);
		extInfo += (string(max(0, 3 - (int64_t)idx.length()), ' ') + idx + "]: Version:[" +
			string(max(0, 2 - (int64_t)specVer.length()), ' ') + specVer + "] Name:[" + extensions[i].extensionName + "]\n");
	}
	ElogI("%s", extInfo.c_str());
#endif
	
	// ����ָ����չ
	//usedExts.push_back("VK_EXT_debug_report");
#ifndef NDEBUG
	usedExts.push_back(VK_DEBUG_UTILS);
#endif
#ifdef WSI_SURF_EXT
	usedExts.push_back(WSI_SURF_EXT);						// �ʹ��ڹ���ϵͳ������surface��չ
	usedExts.push_back(VK_KHR_SURF);
#else
	ElogE("Undefined window system macro: WSI_ SURF_ EXT\n");
	assert(0);
#endif

	// ���֧�ֵ�У���
	uint32_t layerCount;
	CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
	vector<VkLayerProperties> availLayers(layerCount);
	CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, availLayers.data()));
#if COMPILE_MODE == MODE_DEBUG
	string layerInfo("\nAvailable layers[");
	layerInfo += (to_string(layerCount) + "]:\n");
	for (uint32_t i = 0; i < availLayers.size(); i++) {
		layerInfo += "Layer[";
		string idx = to_string(i);
		string specVer = to_string(extensions[i].specVersion);
		layerInfo += (string(max(0, 3 - (int64_t)idx.length()), ' ') + idx + "]: Version:[" +
			string(max(0, 2 - (int64_t)specVer.length()), ' ') + specVer + "] Name:[" + extensions[i].extensionName + "]\n");
	}
	ElogI("%s", layerInfo.c_str());
#endif

#ifndef NDEBUG
	usedLayers.push_back(VK_LAYER_KHR_VAL);
#endif

	// ���ڴ���vkʵ������Ϣ������������Ϣ��ʹ�õ�ȫ����չ��У������Ϣ
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = usedExts.size();
	createInfo.ppEnabledExtensionNames = usedExts.data();
	createInfo.enabledLayerCount = usedLayers.size();
	createInfo.ppEnabledLayerNames = usedLayers.data();

	// ��ʼ��Vulkanʵ��
	CHECK_VK(vkCreateInstance(&createInfo, allocCB, &instance));

#ifndef NDEBUG
	// ע��DEBUG�ص�
	if (!setupDebugExt(instance, &dbgUtilMsgExt)) {
		ElogE("Failed to setupDebugExt\n");
	}
#endif
}

void Factory::getAllPhysicalDevice() {
	// ѡ�������豸
	uint32_t physicalDeviceCount = 0;
	CHECK_VK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
	if (physicalDeviceCount == 0) {
		ElogE("Display adapter count is 0 !");
		assert(0);
	} else {
		ElogI("Physical display device count: %u\n", physicalDeviceCount);
	}
	physicalDevices.resize(physicalDeviceCount);
	CHECK_VK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
}

uint32_t Factory::getPhyDevCmdQueueTypeInfo(VkPhysicalDevice physicalDevice, EVECTOR<VkQueueFamilyProperties>& queueFamiliesProp, EVECTOR<int>& cmdQueueFamilyIndexList) {
	uint32_t maxFamilyQueueCount = 0;
	
	// ��ö�������Ϣ
	uint32_t queueFamliyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamliyCount, nullptr);
	if (queueFamliyCount == 0) {
		ElogE("The number of queue families is 0!");
		assert(0);
	}
	queueFamiliesProp.resize(queueFamliyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamliyCount, queueFamiliesProp.data());

	// ���ڼ�¼���������������ͬʱ���ݵ���������flag����������Խ�࣬����Խ�ߣ�
	// ���ʹ�õ÷���ߵĶ�������Ϊ�������
	int32_t graphicsFamilyScore = -1;
	int32_t computeFamilyScore = -1;
	int32_t transferFamilyScore = -1;
	int32_t protectedFamilyScore = -1;
	int32_t videoDecodeFamilyScore = -1;
	int32_t videoEncodeFamilyScore = -1;
	int32_t opticalFlowFamilyScore = -1;

	// ȷ�ϲ�ͬ����������Ϣ
	cmdQueueFamilyIndexList.resize(7, -1);
	for (uint32_t i = 0; i < queueFamliyCount; i++) {
		maxFamilyQueueCount = maxFamilyQueueCount > queueFamiliesProp[i].queueCount ? maxFamilyQueueCount : queueFamiliesProp[i].queueCount;
#define CHECK_QUEUE_FLAGS(flagBit) score += !!(flags & flagBit);
		auto flags = queueFamiliesProp[i].queueFlags;
		int score = 0;
		
		// ��ô��������
		if ((!(flags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR |
			VK_QUEUE_PROTECTED_BIT | VK_QUEUE_VIDEO_ENCODE_BIT_KHR | VK_QUEUE_OPTICAL_FLOW_BIT_NV)))
			&& (flags & VK_QUEUE_TRANSFER_BIT)) {
			CHECK_QUEUE_FLAGS(VK_QUEUE_SPARSE_BINDING_BIT);
			if (score > transferFamilyScore) {
				cmdQueueFamilyIndexList[Transfer] = i;
				transferFamilyScore = score;
			}
			continue;
		}
		
		CHECK_QUEUE_FLAGS(VK_QUEUE_SPARSE_BINDING_BIT);
		CHECK_QUEUE_FLAGS(VK_QUEUE_VIDEO_DECODE_BIT_KHR);
		CHECK_QUEUE_FLAGS(VK_QUEUE_VIDEO_ENCODE_BIT_KHR);
		CHECK_QUEUE_FLAGS(VK_QUEUE_OPTICAL_FLOW_BIT_NV);

		// ����ܱ����Ķ�����
		if (flags & VK_QUEUE_PROTECTED_BIT) {
			if (score > protectedFamilyScore) {
				protectedFamilyScore = score;
				cmdQueueFamilyIndexList[Protected] = i;
			}
			score += 4;
		} else if (flags & VK_QUEUE_GRAPHICS_BIT) {				// ���ͼ�ζ�����
			if (score > graphicsFamilyScore) {
				graphicsFamilyScore = score;
				cmdQueueFamilyIndexList[Graphics] = i;
			}
			score += 3;
		} else if (flags & VK_QUEUE_COMPUTE_BIT) {				// ��ü��������
			if (score > computeFamilyScore) {
				computeFamilyScore = score;
				cmdQueueFamilyIndexList[Compute] = i;
			}
			score += 2;
		}

		// �����Ƶ���������
		if (flags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
			if (score > videoDecodeFamilyScore) 
				cmdQueueFamilyIndexList[VideoDecode] = i;

		// �����Ƶ���������
		if (flags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
			if (score > videoEncodeFamilyScore)
				cmdQueueFamilyIndexList[VideoEncode] = i;

		// ��ù���������
		if (flags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
			if (score > opticalFlowFamilyScore)
				cmdQueueFamilyIndexList[OpticalFlow] = i;
	}

	// ��������ڲ�����Protected��ͼ�ζ��У�����Protectedͼ�ζ�����Ϊ��ͨͼ�ζ���
	if (cmdQueueFamilyIndexList[Graphics] == -1) {
		if (cmdQueueFamilyIndexList[Protected] == -1) {
			ElogE("No graphic queue family exists");
		} else {
			cmdQueueFamilyIndexList[Graphics] = cmdQueueFamilyIndexList[Protected];
		}
	}

#ifndef NDEBUG
	ElogI("gfx:%d cmpt:%d trans:%d protected:%d decode:%d encode:%d flow:%d\n",
		cmdQueueFamilyIndexList[Graphics], cmdQueueFamilyIndexList[Compute], 
		cmdQueueFamilyIndexList[Transfer], cmdQueueFamilyIndexList[Protected],
		cmdQueueFamilyIndexList[VideoDecode], cmdQueueFamilyIndexList[VideoEncode], 
		cmdQueueFamilyIndexList[OpticalFlow]);
#endif

	return maxFamilyQueueCount;
}

void Factory::delInstance() {
#ifndef NDEBUG
	// ע��DEBUG�ص�
	if (!setdownDebugExt(instance, dbgUtilMsgExt)) {
		ElogE("Failed to setdownDebugExt\n");
	}
#endif

	// �ͷ�Vulkanʵ��
	vkDestroyInstance(instance, EALLOC_CB);

	instance = VK_NULL_HANDLE;
}

void WSISurface::querySurfaceCapabilities(VkSurfaceCapabilitiesKHR* capabilities) {
	if (surface == VK_NULL_HANDLE) {
		ElogE("Surface does not exist.\n");
		return;
	}
	CHECK_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDev, surface, capabilities));
}

EVECTOR<VkSurfaceFormatKHR> WSISurface::querySurfaceFormats() {
	vector<VkSurfaceFormatKHR> surfaceFormats;
	if (surface == VK_NULL_HANDLE) {
		ElogE("Surface does not exist.\n");
	}
	else {
		// ��ѯ������֧�ֵı����ʽ
		uint32_t surfaceFormatCount;
		CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDev, surface, &surfaceFormatCount, nullptr));
		surfaceFormats.resize(surfaceFormatCount);
		CHECK_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDev, surface, &surfaceFormatCount, surfaceFormats.data()));
	}
	return surfaceFormats;
}

EVECTOR<VkPresentModeKHR> WSISurface::queryPresentMode() {
	vector<VkPresentModeKHR> presentModes;
	if (surface == VK_NULL_HANDLE) {
		ElogE("Surface does not exist.\n");
	}
	else {
		// ��ѯ������֧�ֵĳ��ַ�ʽ
		uint32_t presentModeCount;
		CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(phyDev, surface, &presentModeCount, nullptr));
		presentModes.resize(presentModeCount);
		CHECK_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(phyDev, surface, &presentModeCount, presentModes.data()));
	}
	return presentModes;
}

Device::Device(VkDevice device, VkPhysicalDevice phyDevice) :
	device(device), phyDev(phyDevice) {
	getMemoryType();
}

void Device::getMemoryType() {
	// ��������豸�ڴ�����
	vkGetPhysicalDeviceMemoryProperties(phyDev, &memoryProperties);

#if COMPILE_MODE == MODE_DEBUG
	ESTRING memInfo("Physical device infomation:\n");
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		auto& type = memoryProperties.memoryTypes[i];
		auto& heap = memoryProperties.memoryHeaps[type.heapIndex];
		memInfo += "Memory type index:[" + to_string(i) + "] Flags:[" +
			[flags = type.propertyFlags]{
				string flagsStr;
				#define ADD_PROP_STR(prop) if (flags & VK_MEMORY_PROPERTY_##prop##_BIT) flagsStr += " "#prop;
				#define ADD_PROP_STR2(prop, suffix) if (flags & VK_MEMORY_PROPERTY_##prop##_BIT_##suffix) flagsStr += #prop"_"#suffix;
				ADD_PROP_STR(DEVICE_LOCAL);
				ADD_PROP_STR(HOST_VISIBLE);
				ADD_PROP_STR(HOST_COHERENT);
				ADD_PROP_STR(HOST_CACHED);
				ADD_PROP_STR(LAZILY_ALLOCATED);
				ADD_PROP_STR(PROTECTED);
				ADD_PROP_STR2(DEVICE_COHERENT, AMD);
				ADD_PROP_STR2(DEVICE_UNCACHED, AMD);
				ADD_PROP_STR2(RDMA_CAPABLE, NV);
				return flagsStr;
			}() +
			" ] Heap index:[" + to_string(type.heapIndex) + "] Heap size:[" +
			to_string(heap.size) + "] Heap flags:[" +
			[flags = heap.flags]{
			string flagsStr;
			if (flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) flagsStr += " DEVICE_LOCAL";
			if (flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT) flagsStr += " MULTI_INSTANCE";
			return flagsStr;
			}() + " ]\n";
	}
	ElogI("%s", memInfo.c_str());
#endif

	// ���˳�����host�ɷ��ʵ��ڴ����ͺ�����device-local�ڴ�����
	EVECTOR<pair<VkMemoryType, uint32_t>> hostAccessTypes;
	hostAccessTypes.reserve(memoryProperties.memoryTypeCount);
	EVECTOR<pair<VkMemoryType, uint32_t>> deviceLocalType;
	deviceLocalType.reserve(memoryProperties.memoryTypeCount);
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if (memoryProperties.memoryTypes[i].propertyFlags &
			(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
			hostAccessTypes.push_back({ memoryProperties.memoryTypes[i], i });
		if (memoryProperties.memoryTypes[i].propertyFlags &
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			deviceLocalType.push_back({ memoryProperties.memoryTypes[i], i });
		}
	}

	memoryType.resize(3, 0xff);

	// ѡ���ϴ��ѣ�����ѡ���device local������host�ɷ����ұ�����
	EVECTOR<pair<VkMemoryType, uint32_t>> unDevLocTypes;
	for (auto& type : hostAccessTypes) {
		if (memoryProperties.memoryHeaps[type.first.heapIndex].flags == 0)
			unDevLocTypes.push_back(type);
	}
	EVECTOR<pair<VkMemoryType, uint32_t>>* ptr = &unDevLocTypes;
	if (unDevLocTypes.empty()) ptr = &hostAccessTypes;
	memoryType[UploadHeap] = ptr->operator[](0).second;
	for (uint32_t i = 0; i < ptr->size(); i++) {
		if (ptr->operator[](i).first.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
			memoryType[UploadHeap] = ptr->operator[](i).second;
			break;
		}
	}

	// ѡ��Ĭ�϶ѣ�����ѡ��ֻ����device local������
	memoryType[DefaultHeap] = deviceLocalType[0].second;

	// ѡ���ϴ��ѣ�ͬʱ����device local��host�ɷ���
	for (auto& type : deviceLocalType) {
		if (type.first.propertyFlags &
			(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
			memoryType[ReadBackHeap] = type.second;
			break;
		}
	}
	if (memoryType[ReadBackHeap] == 0xff)
		memoryType[ReadBackHeap] = memoryType[UploadHeap];

	heapInfos.resize(3);
#define GET_HEAP_INFO(x) heapInfos[x].size = memoryProperties.memoryHeaps[memoryType[x]].size; \
	heapInfos[x].heapFlags = memoryProperties.memoryHeaps[memoryType[x]].flags; \
	heapInfos[x].propertyFlags = memoryProperties.memoryTypes[memoryType[x]].propertyFlags;
	GET_HEAP_INFO(UploadHeap);
	GET_HEAP_INFO(DefaultHeap);
	GET_HEAP_INFO(ReadBackHeap);

#if COMPILE_MODE == MODE_DEBUG
	ElogI("Native memory type index: UploadHeap[%d] DefaultHeap[%d] ReadBackHeap[%d]\n",
		memoryType[UploadHeap], memoryType[DefaultHeap], memoryType[ReadBackHeap]);
#endif
}

WSISurface* Device::createSurface(NativeSurface* nativeSurface) {
	VkSurfaceKHR surface;
	CHECK_INST;
#ifdef GLFW
	CHECK_VK(glfwCreateWindowSurface(EINSTANCE, nativeSurface->nativeWindow, EALLOC_CB, &surface));
#elif WAYLAND

#endif
	
	return new WSISurface(surface, phyDev, device);
}

bool Device::checkCommandQueuePrensent(WSISurface* surface, CommandObjectType cmdQueueType) {
	if (surface->device != this->device) {
		ElogE("Device that create the WSI surface is not current Device.\n");
		return false;
	}

	VkBool32 presentSupport;
	CHECK_VK(vkGetPhysicalDeviceSurfaceSupportKHR(phyDev, cmdQueueFamilyIndexList[cmdQueueType],surface->surface, &presentSupport));

	return presentSupport;
}

ShaderFile* Device::readShaderFile(const char* sourceCodePath) {
	auto shaderCode = readFile(sourceCodePath);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
	VkShaderModule shaderModule;
	CHECK_VK(vkCreateShaderModule(device, &createInfo, EALLOC_CB, &shaderModule));

	return new ShaderFile(shaderModule, device);
}

PipelineLayout* Device::createPipelineLayout(EVECTOR<VkDescriptorSetLayout>* descSetLayouts, EVECTOR<VkPushConstantRange>* pushConstantRange, VkPipelineLayoutCreateFlags flag) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.flags = VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT;
	if (descSetLayouts == nullptr) {
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
	} else {
		pipelineLayoutInfo.setLayoutCount = descSetLayouts->size();
		pipelineLayoutInfo.pSetLayouts = descSetLayouts->data();
	}
	if (pushConstantRange == nullptr) {
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
	} else {
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantRange->size();
		pipelineLayoutInfo.pPushConstantRanges = pushConstantRange->data();
	}

	VkPipelineLayout pipelineLayout;
	CHECK_VK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, EALLOC_CB, &pipelineLayout));

	return new PipelineLayout(pipelineLayout, device);
}

RenderPass* Device::createRenderPass(EVECTOR<VkAttachmentDescription>* attachments, EVECTOR<VkSubpassDescription>* subpasses, EVECTOR<VkSubpassDependency>* dependecies) {
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments->size();
	renderPassInfo.pAttachments = attachments->data();
	renderPassInfo.subpassCount = subpasses->size();
	renderPassInfo.pSubpasses = subpasses->data();
	renderPassInfo.dependencyCount = dependecies->size();
	renderPassInfo.pDependencies = dependecies->data();
	
	VkRenderPass renderPass;
	CHECK_VK(vkCreateRenderPass(device, &renderPassInfo, EALLOC_CB, &renderPass));

	return new RenderPass(renderPass, device);
}

ImageView* Device::createSwapchainImageView(Image* img, VkFormat format, VkComponentMapping* mapping) {
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = img->image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;						// 2ά����
	createInfo.format = format;
	if (mapping == nullptr) {
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	} else {
		createInfo.components = *mapping;
	}
	// ָ��ͼ����;��ͼ����һ���ֿɱ����ʣ��˴�ͼ��������ȾĿ�꣬û��ϸ�ּ���ֻ��һ��ͼ��
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imgView;
	CHECK_VK(vkCreateImageView(device, &createInfo, EALLOC_CB, &imgView));

	return new ImageView(imgView);
}

SwapChain* WSISurface::createSwapChain(SwapchainCreateInfo* info, CommandQueue* cmdQueue) {
	if (info == nullptr) {
		ElogE("Param \"info\" is nullptr.\n");
		return nullptr;
	}

	// ���������������ػ��壩
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = info->buffersCount;									// ����������ʹ��������
	swapchainCreateInfo.imageFormat = info->format.format;							// surface��ʽ
	swapchainCreateInfo.imageColorSpace = info->format.colorSpace;					// ��ɫ�ռ�
	swapchainCreateInfo.imageExtent = { info->width, info->height };				// surface�ֱ���
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;				// ����������;���˴����ڸ��ŵ���ɫ���壬��Ҫ��ͼ����к�����ʹ��VK_IAMGE_SUAGE_TRANSFER_DST_BIT
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;					// ���ƶ�����ж�ͼ��ķ��ʷ�ʽ
	swapchainCreateInfo.queueFamilyIndexCount = 0;										// �б��湲��Ȩ�Ķ���������
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;									// �б��湲��Ȩ��������
	swapchainCreateInfo.preTransform = info->transMode;										// ָ��һ���̶��ı任����
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;				// ָ��alphaͨ���Ƿ������ʹ���ϵͳ���������ڽ��л�ϲ�������ʱ����alphaͨ��
	swapchainCreateInfo.presentMode = info->presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;												// ��ʾ����ϵ������ϵͳ�����������ڵ���������ɫ��������Vulkan��ȡһ�����Ż���ʩ�����ض���������ֵ���ܳ�����
	swapchainCreateInfo.oldSwapchain =													// �ʹ��ڸı��뽻�����ؽ��й�
		[info] {
		if (info->oldSwapchain) {
			return info->oldSwapchain->swapchain;
		}
		return (VkSwapchainKHR)VK_NULL_HANDLE;
		}();
	VkSwapchainKHR swapchain;
	CHECK_VK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, EALLOC_CB, &swapchain));

	auto sp = new SwapChain(swapchain, info->buffersCount, info->format.format, cmdQueue->queue, device);

	// ��ý�����ͼ��
	uint32_t imgCount;
	sp->images.resize(info->buffersCount);
	CHECK_VK(vkGetSwapchainImagesKHR(device, sp->swapchain, &imgCount, sp->images.data()));

	return sp;
}

Resource* Device::createResource(uint64_t size, DeviceMemoryType type, uint32_t vkMemoryType) {
	VkMemoryAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	if (type == CustomHeap) {
		info.memoryTypeIndex = vkMemoryType;
	} else {
		info.memoryTypeIndex = memoryType[type];
	}
	info.allocationSize = size;
	VkDeviceMemory memory;
	CHECK_VK(vkAllocateMemory(device, &info, EALLOC_CB, &memory));

	return new Resource(memory, size, type,
		memoryProperties.memoryHeaps[memoryProperties.memoryTypes[info.memoryTypeIndex].heapIndex].flags,
		memoryProperties.memoryTypes[info.memoryTypeIndex].propertyFlags,
		device, this);
}

CommandQueue* Device::getCommandQueue(CommandObjectType queueType, uint32_t queueIndex) {
	if (cmdQueueFamilyIndexList[queueType] == -1) {
		ElogE("%s queue is not supported.", CommandTypeStr[queueType]);
		return nullptr;
	}

	VkQueue vkQueue;
	vkGetDeviceQueue(device, cmdQueueFamilyIndexList[queueType], queueIndex, &vkQueue);

	return (new CommandQueue(vkQueue));
}

CommandAllocator* Device::createCommandAllocator(CommandObjectType type, CommandAllocatorFlag flags) {
	if (cmdQueueFamilyIndexList[type] == -1) {
		ElogE("%s command allocator is not supported.", CommandTypeStr[type]);
		return nullptr;
	}
	
	VkCommandPoolCreateInfo poolcreateInfo = {};
	poolcreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolcreateInfo.queueFamilyIndex = cmdQueueFamilyIndexList[type];
	poolcreateInfo.flags = flags;
	VkCommandPool cmdPool;
	CHECK_VK(vkCreateCommandPool(device, &poolcreateInfo, EALLOC_CB, &cmdPool));
	
	return new CommandAllocator(cmdPool, device, type);
}

GraphicsPipeline* Device::createGraphicsPipeline(PipeCreateInfo& createInfos, EVECTOR<ShaderStage>& shaderStages, PipelineLayout* layout, RenderPass* renderPass, uint32_t subpassIndex) {
	uint32_t stageCount = shaderStages.size();
	EVECTOR<VkPipelineShaderStageCreateInfo> stages(stageCount);
	for (uint32_t i = 0; i < stageCount; i++) {
		stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[i].stage = (VkShaderStageFlagBits)shaderStages[i].stage;
		stages[i].module = shaderStages[i].shaderFile->shaderModule;
		stages[i].pSpecializationInfo = nullptr;
		stages[i].pName = shaderStages[i].funcName;
		stages[i].flags = shaderStages[i].flags;
	}

	VkGraphicsPipelineCreateInfo vkPipeInfo = {};
	vkPipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkPipeInfo.stageCount = stageCount;
	vkPipeInfo.pStages = stages.data();
	vkPipeInfo.pVertexInputState = createInfos.vertexInputState;
	vkPipeInfo.pInputAssemblyState = createInfos.inputAssemblyState;
	vkPipeInfo.pViewportState = createInfos.viewportState;
	vkPipeInfo.pRasterizationState = createInfos.rasterizationState;
	vkPipeInfo.pMultisampleState = createInfos.multisampleState;
	vkPipeInfo.pDepthStencilState = createInfos.depthStencilState;
	vkPipeInfo.pColorBlendState = createInfos.colorBlendState;
	vkPipeInfo.pDynamicState = createInfos.dynamicState;
	vkPipeInfo.layout = layout->pipelineLayout;
	vkPipeInfo.renderPass = renderPass->renderPass;
	vkPipeInfo.subpass = subpassIndex;
	vkPipeInfo.basePipelineHandle = VK_NULL_HANDLE;							// �������������������Ƿ���һ���Ѵ����õĹ��߻����ϴ���һ���¹��ߣ���ô�������Ĵ���С���л�Ч�ʸ�
	vkPipeInfo.basePipelineIndex = -1;										// Ŀǰ���ã�������ô�裬��������ֻ��VkGraphicsPipelineCreateInfo::flagsʹ��VK_PIPELINE_CREATE_DERIVATIVE_BIT����Ч
	VkPipeline graphicsPipeline;
	CHECK_VK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &vkPipeInfo, EALLOC_CB, &graphicsPipeline));

	return new GraphicsPipeline(graphicsPipeline);
}

FrameBuffer* Device::createFrameBuffer(RenderPass* renderPass, EVECTOR<ImageView*>& attachments, uint32_t width, uint32_t height, uint32_t layers ) {
	uint32_t attachsCount = attachments.size();
	EVECTOR<VkImageView> imgViews(attachsCount);
	for (uint32_t i = 0; i < attachsCount; i++) {
		imgViews[i] = attachments[i]->imgView;
	}
	
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass->renderPass;
	framebufferInfo.attachmentCount = attachsCount;
	framebufferInfo.pAttachments = imgViews.data();
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = layers;
	VkFramebuffer fb;
	CHECK_VK(vkCreateFramebuffer(device, &framebufferInfo, EALLOC_CB, &fb));

	return new FrameBuffer(fb, device);
}

Semaphore* Device::createSemaphore(VkSemaphoreCreateFlags flags) {
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.flags = flags;
	VkSemaphore semaphore;
	CHECK_VK(vkCreateSemaphore(device, &createInfo, EALLOC_CB, &semaphore));

	return new Semaphore(semaphore);
}

Fence* Device::createFence(VkFenceCreateFlags flags) {
	VkFenceCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.flags = flags;
	VkFence fence;
	vkCreateFence(device, &info, EALLOC_CB, &fence);

	return new Fence(fence, device);
}

void Device::waitForFences(EVECTOR<Fence*>& fences, bool waitAll, uint64_t timeout) {
	uint32_t fenceCount = fences.size();
	if (fenceCount == 0) {
		ElogE("The number of waiting fences is 0.\n");
		assert(0);
	}
	EVECTOR<VkFence> vkFences(fenceCount);
	for (uint32_t i = 0; i < fenceCount; i++) {
		vkFences[i] = fences[i]->fence;
	}
	vkWaitForFences(device, fenceCount, vkFences.data(), waitAll, timeout);
}

VkPhysicalDeviceMemoryProperties Device::queryVkDeviceMemory() {
	return memoryProperties;
}

Buffer* Device::createBuffer(DeviceMemoryType type, VkBufferUsageFlags usage, uint64_t size, EVECTOR<CommandObjectType>* sharedCmdType, uint32_t vkMemoryType) {
	// ����VkBuffer
	EVECTOR<uint32_t> sharingQueueFamilesIdxs;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	if (sharedCmdType == nullptr || sharedCmdType->size() == 0) {
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	} else {
		uint32_t sharedCmdTypeCount = sharedCmdType->size();
		sharingQueueFamilesIdxs.resize(sharedCmdTypeCount);
		for (uint32_t i = 0; i < sharedCmdTypeCount; i++)
			sharingQueueFamilesIdxs[i] = cmdQueueFamilyIndexList[sharedCmdType->operator[](i)];
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount = sharedCmdTypeCount;
		bufferInfo.pQueueFamilyIndices = sharingQueueFamilesIdxs.data();
	}
	VkBuffer buffer;
	CHECK_VK(vkCreateBuffer(device, &bufferInfo, EALLOC_CB, &buffer));

	// ���Bufferʵ����Ҫ�Ĵ�С
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(device, buffer, &memReqs);
	
	// ����VkDeviceMemory
	VkMemoryAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	if (type == CustomHeap) {
		info.memoryTypeIndex = vkMemoryType;
	} else {
		info.memoryTypeIndex = memoryType[type];
	}
	info.allocationSize = memReqs.size;
	VkDeviceMemory memory;
	CHECK_VK(vkAllocateMemory(device, &info, EALLOC_CB, &memory));

	// ��VkBuffer�󶨵�VkDeviceMemory��
	CHECK_VK(vkBindBufferMemory(device, buffer, memory, 0));

	return new BufferWithResource(memory, buffer, type, info.memoryTypeIndex,
		memoryProperties.memoryHeaps[memoryProperties.memoryTypes[info.memoryTypeIndex].heapIndex].flags,
		device, size, memReqs.size);
}

void CommandList::beginRecord(VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo* pInheritanceInfo) {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags;
	beginInfo.pInheritanceInfo = pInheritanceInfo;
	CHECK_VK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
}

void CommandList::beginRenderPass(RenderPass* renderPass, FrameBuffer* framebuffer, VkRect2D& renderArea, EVECTOR<VkClearValue>& clearValues, VkSubpassContents subpassContents) {

	VkRenderPassBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass = renderPass->renderPass;
	beginInfo.framebuffer = framebuffer->framebuffer;
	beginInfo.renderArea = renderArea;
	beginInfo.clearValueCount = clearValues.size();
	beginInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(cmdBuffer, &beginInfo, subpassContents);
}

void CommandQueue::execute(EVECTOR<ExecuteInfo>& submitInfos, Fence* fence) {
	uint32_t submitCount = submitInfos.size();
	EVECTOR<VkSubmitInfo> vkSubmitInfos(submitCount, VkSubmitInfo{});
	EVECTOR<EVECTOR<VkSemaphore>> waitSemaphores(submitCount);
	EVECTOR<EVECTOR<VkSemaphore>> signalSemaphores(submitCount);
	EVECTOR<EVECTOR<VkCommandBuffer>> cmdBuffers(submitCount);
	for (uint32_t i = 0; i < submitCount; i++) {
		vkSubmitInfos[i].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// �ȴ��ź�������
		uint32_t waitSemaphoreCount = submitInfos[i].waitSemaphores.size();
		waitSemaphores[i].resize(waitSemaphoreCount);
		for (int j = 0; j < waitSemaphoreCount; j++) {
			waitSemaphores[i][j] = submitInfos[i].waitSemaphores[j]->semaphore;
		}
		vkSubmitInfos[i].waitSemaphoreCount = waitSemaphoreCount;
		vkSubmitInfos[i].pWaitSemaphores = waitSemaphores[i].data();
		vkSubmitInfos[i].pWaitDstStageMask = submitInfos[i].waitStages.data();
		// �����ź�������
		uint32_t signalSemaphoreCount = submitInfos[i].signalSemaphore.size();
		signalSemaphores[i].resize(signalSemaphoreCount);
		for (int j = 0; j < signalSemaphoreCount; j++) {
			signalSemaphores[i][j] = submitInfos[i].signalSemaphore[j]->semaphore;
		}
		vkSubmitInfos[i].signalSemaphoreCount = signalSemaphoreCount;
		vkSubmitInfos[i].pSignalSemaphores = signalSemaphores[i].data();
		// ���ύ����
		uint32_t CommandListCount = submitInfos[i].commandLists.size();
		cmdBuffers[i].resize(CommandListCount);
		for (int j = 0; j < CommandListCount; j++) {
			cmdBuffers[i][j] = submitInfos[i].commandLists[j]->cmdBuffer;
		}
		vkSubmitInfos[i].commandBufferCount = CommandListCount;
		vkSubmitInfos[i].pCommandBuffers = cmdBuffers[i].data();
	}
	CHECK_VK(vkQueueSubmit(queue, submitCount, vkSubmitInfos.data(),
		[fence] {
			if (fence) {
				return fence->fence;
			}
			else {
				return (VkFence)VK_NULL_HANDLE;
			}
		}()
		)
	);
}

GraphicsCommandList* CommandAllocator::createGraphicsCommandList(VkCommandBufferLevel level) {
	if (type != Graphics) {
		ElogE("CommandAllocator whose type is not Graphics can't create graphics commandList.\n");
		return nullptr;
	}

	VkCommandBuffer cmdBuffer;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = 1;
	CHECK_VK(vkAllocateCommandBuffers(device, &allocInfo, &cmdBuffer));

	return new GraphicsCommandList(cmdBuffer);
}

void* Resource::map(uint64_t size, uint64_t offset) {
	if (!(propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		ElogE("Resource whose type is %s and native type is 0x%x can't be mapped.\n", 
			deviceMemoryTypeStr[this->type], propertyFlags);
		return nullptr;
	}
	if (isMapped) {
		ElogE("Resource has already been mapped and cannot be mapped again.\n");
		return nullptr;
	}
	if (size != 0 && ((offset + size) > this->size)) {
		ElogE("The area[size:%lld offset:%lld] to be mapped exceeds the Resource range[size:%lld].\n",
			size, offset, this->size);
		return nullptr;
	}

	void* data;
	CHECK_VK(vkMapMemory(vkDevice, memory, size == 0 ? 0 : offset, size == 0 ? this->size : 0, 0, &data));
	isMapped == true;

	return data;
}

bool Resource::unmap() {
	if (!isMapped) {
		ElogE("Resource is not mapped and cannot be unmapped.\n");
		return false;
	}

	vkFreeMemory(vkDevice, memory, EALLOC_CB);
	isMapped = false;

	return true;
}

bool Resource::copy(void* srcData, uint64_t size, uint64_t offset) {
	if (!(propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		ElogE("Resource whose memory type is %s and native type is 0x%x can't be copied.\n",
			deviceMemoryTypeStr[type], propertyFlags);
		return false;
	}
	if (isMapped) {
		ElogE("Resource has already been mapped and cannot be copied.\n");
		return false;
	}
	if (size != 0 && ((offset + size) > this->size)) {
		ElogE("The area[size:%lld offset:%lld] to be copied exceeds Buffer range[size:%lld].\n",
			size, offset, this->size);
		return false;
	}

	void* dstData;
	uint64_t realCopySize = size == 0 ? this->size : 0;
	uint64_t realCopyOffset = size == 0 ? 0 : offset;
	CHECK_VK(vkMapMemory(vkDevice, memory, realCopyOffset, realCopySize, 0, &dstData));

	memcpy(dstData, srcData, realCopySize);

	vkFreeMemory(vkDevice, memory, EALLOC_CB);

	return true;
}

Buffer* Resource::createBuffer(VkBufferUsageFlags usage, uint64_t size, uint64_t offset, EVECTOR<CommandObjectType>* sharedCmdType) {
	if ((size + offset) > this->size) {
		ElogE("Failed to create Buffer because parameter \"size\"[%lld] + \"offset\[%lld] > resource size[%dlld]",
			size, offset, this->size);
		return nullptr;
	}

	EVECTOR<uint32_t> sharingQueueFamilesIdxs;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	if (sharedCmdType == nullptr || sharedCmdType->size() == 0) {
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	} else {
		uint32_t sharedCmdTypeCount = sharedCmdType->size();
		sharingQueueFamilesIdxs.resize(sharedCmdTypeCount);
		for (uint32_t i = 0; i < sharedCmdTypeCount; i++)
			sharingQueueFamilesIdxs[i] = device->cmdQueueFamilyIndexList[sharedCmdType->operator[](i)];

		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount = sharedCmdTypeCount;
		bufferInfo.pQueueFamilyIndices = sharingQueueFamilesIdxs.data();
	}

	VkBuffer buffer;
	CHECK_VK(vkCreateBuffer(vkDevice, &bufferInfo, EALLOC_CB, &buffer));

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(vkDevice, buffer, &memReqs);
	uint64_t realOffset = (offset + memReqs.alignment - 1) & ~(memReqs.alignment - 1);
	if ((realOffset + memReqs.size) > this->size) {
		ElogE("Resource does not have enough space to allocate buffer. \
			Buffer aligment:[%lld] real offset:[%lld] real size:[%lld] Resource size:[%lld]\n",
			memReqs.alignment, realOffset, memReqs.size, this->size);
		vkDestroyBuffer(vkDevice, buffer, EALLOC_CB);
		return nullptr;
	}

	CHECK_VK(vkBindBufferMemory(vkDevice, buffer, memory, realOffset));

	return new BufferResource(buffer, vkDevice, size, memReqs.size, offset, realOffset, this);
}

void* BufferResource::map(uint64_t size, uint64_t offset) {
	if (!(resource->propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		ElogE("Buffer whose memory type is %s and native type is 0x%x can't be mapped.\n",
			deviceMemoryTypeStr[resource->type], resource->propertyFlags);
		return nullptr;
	}
	if (resource->isMapped) {
		ElogE("Buffer has already been mapped and cannot be mapped again.\n");
		return nullptr;
	}
	if (size != 0 && ((offset + size) > this->size)) {
		ElogE("The area[size:%lld offset:%lld] to be mapped exceeds Buffer range[size:%lld].\n",
			size, offset, this->size);
		return nullptr;
	}

	void* data;
	CHECK_VK(vkMapMemory(device, resource->memory, (size == 0 ? 0 : offset) + this->realOffset, size == 0 ? this->size : size, 0, &data));
	resource->isMapped == true;

	return data;
}

bool BufferResource::unmap() {
	if (!resource->isMapped) {
		ElogE("Buffer is not mapped and cannot be unmapped.\n");
		return false;
	}

	vkFreeMemory(device, resource->memory, EALLOC_CB);
	resource->isMapped = false;

	return true;
}

bool BufferResource::copy(void* srcData, uint64_t size, uint64_t offset) {
	if (!(resource->propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		ElogE("Buffer whose memory type is %s and native type is 0x%x can't be copied.\n",
			deviceMemoryTypeStr[resource->type], resource->propertyFlags);
		return false;
	}
	if (resource->isMapped) {
		ElogE("Buffer has already been mapped and cannot be copied again.\n");
		return false;
	}
	if (size != 0 && ((offset + size) > this->size)) {
		ElogE("The area[size:%lld offset:%lld] to be copied exceeds Buffer range[size:%lld].\n", 
			size, offset, this->size);
		return false;
	}

	void* dstData;
	uint64_t realCopySize = size == 0 ? this->size : 0;
	uint64_t realCopyOffset = (size == 0 ? 0 : offset) + realOffset;
	CHECK_VK(vkMapMemory(device, resource->memory, realCopyOffset, realCopySize, 0, &dstData));

	memcpy(dstData, srcData, realCopySize);

	vkFreeMemory(device, resource->memory, EALLOC_CB);
}

void* BufferWithResource::map(uint64_t size, uint64_t offset) {
	if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		ElogE("Buffer whose memory type is %s and native type is 0x%x can't be mapped.\n",
			deviceMemoryTypeStr[type], propertyFlags);
		return nullptr;
	}
	if (isMapped) {
		ElogE("Buffer has already been mapped and cannot be mapped again.\n");
		return nullptr;
	}
	if (size != 0 && ((offset + size) > this->size)) {
		ElogE("The area[size:%lld offset:%lld] to be mapped exceeds Buffer range[size:%lld].\n",
			size, offset, this->size);
		return nullptr;
	}

	void* data;
	CHECK_VK(vkMapMemory(device, memory, (size == 0 ? 0 : offset), size == 0 ? this->size : size, 0, &data));

	return data;
}

bool BufferWithResource::unmap() {
	if (!isMapped) {
		ElogE("Buffer is not mapped and cannot be unmapped.\n");
		return false;
	}

	vkFreeMemory(device, memory, EALLOC_CB);
	isMapped = false;

	return true;
}

bool BufferWithResource::copy(void* srcData, uint64_t size, uint64_t offset) {
	if (!(propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
		ElogE("Buffer whose memory type is %s and native type is 0x%x can't be copied.\n",
			deviceMemoryTypeStr[type], propertyFlags);
		return false;
	}
	if (isMapped) {
		ElogE("Buffer has already been mapped and cannot be copied again.\n");
		return false;
	}
	if (size != 0 && ((offset + size) > this->size)) {
		ElogE("The area[size:%lld offset:%lld] to be copied exceeds Buffer range[size:%lld].\n",
			size, offset, this->size);
		return false;
	}

	void* dstData;
	uint64_t copySize = size == 0 ? this->size : 0;
	uint64_t copyOffset = size == 0 ? 0 : offset;
	CHECK_VK(vkMapMemory(device, memory, copyOffset, copySize, 0, &dstData));

	memcpy(dstData, srcData, copySize);

	vkFreeMemory(device, memory, EALLOC_CB);
}

VkPipeline GraphicsPipeline::getVkPipeline() {
	return pipeline;
}

Image* SwapChain::getBuffer(uint32_t index) {
	return new ImageSwapChain(images[index]);
}

uint32_t SwapChain::getNextImageIndex(Semaphore* semaphore, Fence* fence, uint64_t timeout) {
	uint32_t imgIdx;
	CHECK_VK(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, 
		[semaphore] { return semaphore == nullptr ? VK_NULL_HANDLE : semaphore->semaphore; }(),
		[fence] {return fence == nullptr ? VK_NULL_HANDLE : fence->fence; }(),
		&imgIdx));
	return imgIdx;
}

void SwapChain::present(uint32_t imageIndex, EVECTOR<Semaphore*>* semaphores) {
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	EVECTOR<VkSemaphore> vkSemaphores;
	if (semaphores != nullptr) {
		uint32_t semaphoreCount = semaphores->size();
		vkSemaphores.resize(semaphoreCount);
		for (int i = 0; i < semaphoreCount; i++) {
			vkSemaphores[i] = semaphores->operator[](i)->semaphore;
		}
		info.waitSemaphoreCount = semaphoreCount;
		info.pWaitSemaphores = vkSemaphores.data();
	}
	info.swapchainCount = 1;
	info.pSwapchains = &swapchain;
	info.pImageIndices = &imageIndex;
	CHECK_VK(vkQueuePresentKHR(queue, &info));
}

