#pragma once

//#define NDEBUG

#ifndef VK_BASE
#define VK_BASE

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <stdint.h>

#include "VkXTool.h"

#define GLFW
#ifdef GLFW
#include <glfw3.h>
#ifdef WIN32
#define WSI_SURF_EXT "VK_KHR_win32_surface"
#endif
#elif WAYLAND
#define WSI_SURF_EXT ""
#endif

#ifdef USE_ALIOS_LOG
#define logI LOG_I
#define logW LOG_W
#define logE LOG_E
#else
#define logI printf
#define logW printf
#define logE printf
#endif

#define EVECTOR std::vector
#define ESTRING std::string

#define EINSTANCE Factory::instance
#define EALLOC_CB Factory::allocCB

// ELOGx�Ƿ�װ��ר��LOG����ʹ������ģʽ�����LOG�ر�
#define ElogI(format, ...)
#define ElogW(format, ...)
#define ElogE(format, ...)

// �����ֱ���ģʽ���ֱ�������ģʽ MODE_PERFORMANCE����ͨģʽ��MODE_NORMAL������ģʽ MODE_DEBUG
// ����ģʽ���vulkan���Բ㣬����ģʽ��رռ�������log����Ҫ��Elog)
#define MODE_PERFORMANCE 0
#define MODE_NORMAL 1
#define MODE_DEBUG 2

#define COMPILE_MODE MODE_DEBUG

#if COMPILE_MODE > MODE_PERFORMANCE
#undef ElogI(format, ...)
#undef ElogW(format, ...)
#undef ElogE(format, ...)
#define ElogI(...) logI("Object[%s]:Function:[%s]: ", this->name.c_str(), THE_FUNC); logI(__VA_ARGS__);
#define ElogW(...) logW("Object[%s]:Function:[%s]: ", this->name.c_str(), THE_FUNC); logI(__VA_ARGS__);
#define ElogE(...) logE("Object[%s]:Function:[%s]: ", this->name.c_str(), THE_FUNC); logI(__VA_ARGS__);
#endif
#if COMPILE_MODE < MODE_DEBUG
#define NDEBUG
#endif

#include <cassert>

#ifndef NDEBUG
// ��鷵������Ϊ VkResult ��vk��������ֵ
#define CHECK_VK(func) { VkResult ret = func; if (ret != VK_SUCCESS) { logI("Failed in File: %s Line: %d\n", THE_FILE, THE_LINE); checkErrCode(ret); assert(0); } }

#define CHECK_INST assert(EINSTANCE != VK_NULL_HANDLE);

// ����VK_EXT_debug_tuilsУ�����չ��ע��ص�����
bool setupDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT* callback);

// ע���ص�����
bool setdownDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT callback);

void checkErrCode(VkResult ret);

#else
#define CHECK_INST
#define CHECK_VK(func) {func;}
#endif

#define THE_LINE __LINE__
#define THE_FILE __FILE__
#define THE_FUNC __func__

class  BaseObject;
class  Factory;
class  WSISurface;
class  Device;
class  CommandQueue;
class  CommandAllocator;
class  CommandList;
class  GraphicsCommandList;
class  Image;
class  ImageSwapChain;
class  ImageView;
class  ShaderFile;
class  PipelineLayout;
class  SwapChain;
class  Resource;
class  Buffer;
class  BufferResource;
class  BufferWithResource;
class  FrameBuffer;
class  RenderPass;
class  Pipeline;
class  GraphicsPipeline;
class  Semaphore;
class  Fence;
struct ShaderStage;

enum CommandObjectType {
	Graphics = 0,
	Compute = 1,
	Transfer = 2,
	Protected = 3,
	VideoDecode = 4,
	VideoEncode = 5,
	OpticalFlow = 6
};

enum CommandAllocatorFlag {
	Default = 0,
	Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
	IndvReset = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
};

enum PipelineShaderStage {
	VertexShaderStage = VK_SHADER_STAGE_VERTEX_BIT,
	FragmentShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT
};

enum DeviceMemoryType {
	UploadHeap = 0,
	DefaultHeap = 1,
	ReadBackHeap = 2,
	CustomHeap = 3
};

// �Զ���һ��ָ��ת��Ϊ����ָ��
template<typename T>
EINLINE std::shared_ptr<T> ComPtr(T* x) {
	return std::shared_ptr<T>(x);
}

struct NativeSurface {
	uint32_t width = 0;
	uint32_t height = 0;
#ifdef GLFW
	GLFWwindow* nativeWindow = nullptr;
#else WAYLAND
	
#endif;
};

struct ShaderStage{
	ShaderFile* shaderFile;
	PipelineShaderStage				 stage;
	const char* funcName = "main";
	VkPipelineShaderStageCreateFlags flags = 0;
	const VkSpecializationInfo* specializationInfo = nullptr;
	ShaderStage() {}
	ShaderStage(ShaderFile* shaderFile, PipelineShaderStage stage) : shaderFile(shaderFile), stage(stage) {}
};

struct HeapProperty {
	uint64_t											size;
	VkMemoryHeapFlags									flags;
	EVECTOR<std::pair<VkMemoryPropertyFlags, uint32_t>>	memoryTypes;
};

struct EngineInitInfo {
	std::string appName;
	std::string engineName;
};

// ����������������Ϣ
struct SwapchainCreateInfo {
	uint32_t						width;														// ���ڿ�
	uint32_t						height;														// ���ڸ�
	uint32_t						buffersCount = 3;											// ��������������
	VkSurfaceFormatKHR				format;														// ������ͼ���ʽ
	VkPresentModeKHR				presentMode = VK_PRESENT_MODE_MAILBOX_KHR;					// ����������ģʽ
	VkSurfaceTransformFlagBitsKHR	transMode = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;			// ͼ�����ǰ�仯ģʽ
	VkCompositeAlphaFlagBitsKHR		alphaUsage = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;				// alphaͨ���ô�
	SwapChain*						oldSwapchain = nullptr;								// �Ͻ�����
};

struct PipeCreateInfo {
	VkPipelineCreateFlags					flags = 0;
	VkPipelineVertexInputStateCreateInfo*	vertexInputState;
	VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState;
	VkPipelineTessellationStateCreateInfo*	tessellationState;
	VkPipelineViewportStateCreateInfo*		viewportState;
	VkPipelineRasterizationStateCreateInfo* rasterizationState;
	VkPipelineMultisampleStateCreateInfo*	multisampleState;
	VkPipelineDepthStencilStateCreateInfo*	depthStencilState;
	VkPipelineColorBlendStateCreateInfo*	colorBlendState;
	VkPipelineDynamicStateCreateInfo*		dynamicState;
};

struct ExecuteInfo{
	EVECTOR<CommandList*>			commandLists;
	EVECTOR<Semaphore*>				waitSemaphores;
	EVECTOR<Semaphore*>				signalSemaphore;
	EVECTOR<VkPipelineStageFlags>	waitStages;
};

class BaseObject {
public:
	void setName(ESTRING& name) { this->name = name; }

protected:
	ESTRING name{ "unnamed" };
};

class Factory : public BaseObject {
public:
	static Factory* createFactory(EngineInitInfo* info, VkAllocationCallbacks* allocCB = nullptr);

	// �鿴�豸����
	uint64_t getAdapterCount() { return physicalDevices.size(); }

	// ��ѯ�����豸��Ϣ
	// @param[in] index: ����ѯ�豸������
	// @param[out] info: ���ڷ����豸��Ϣ
	// @return �ɹ�����true����֮false
	bool getAdapterInfo(uint32_t phyDevIdx, VkPhysicalDeviceProperties* info);

	//  ��ѯ�����豸����
	// @param[in] index: ����ѯ�豸������
	// @param[out] capacities: ���ڷ����豸����
	// @return �ɹ�����true����֮false
	bool getAdapterCapacities(uint32_t phyDevIdx, VkPhysicalDeviceFeatures* capacities);

	// �����߼��豸������ͼ�Ρ����㡢���䡢��Ƶ������У�������ϼ��߶��еĻ������������ȼ���Ϊ1
	// @param[in] phyDevIdx: ���߼��豸�󶨵������豸����
	// @return ���ش������߼��豸
	Device* createDevice(uint32_t phyDevIdx);

	virtual ~Factory();

private:
	Factory(EngineInitInfo* info, VkAllocationCallbacks* allocCB);

	// ����Vulkanʵ��
	void createVkInstance();

	// ���ȫ�������豸��Ϣ
	void getAllPhysicalDevice();

	// ��������豸����������Ϣ
	uint32_t getPhyDevCmdQueueTypeInfo(VkPhysicalDevice physicalDevice, EVECTOR<VkQueueFamilyProperties>& queueFamiliesProp, EVECTOR<int>& cmdQueueFamilyIndexList);

	// ����ʵ��
	void delInstance();

private:
	static Factory*				  myself;
	static VkInstance			  instance;
	static VkAllocationCallbacks* allocCB;
								  
	std::string					  appName;
	std::string					  engineName;
	uint32_t					  apiVersion;
								  
	VkDebugUtilsMessengerEXT	  dbgUtilMsgExt;
								     
	// ��ѯ�������������豸		   
	EVECTOR<VkPhysicalDevice>	  physicalDevices;

	// ��¼��ͬ�����豸�ڴ��Ӧ��vk�豸�ڴ�����
	EVECTOR<uint32_t>			  memoryType;
								  
	EVECTOR<const char*>		  usedExts;
	EVECTOR<const char*>		  usedLayers;

	friend class Device;
	friend class WSISurface;
	friend class CommandQueue;
	friend class CommandAllocator;
	friend class CommandList;
	friend class GraphicsCommandList;
	friend class Image;
	friend class ImageSwapChain;
	friend class ImageView;
	friend class ShaderFile;
	friend class PipelineLayout;
	friend class SwapChain;
	friend class Resource;
	friend class FrameBuffer;
	friend class RenderPass;
	friend class Pipeline;
	friend class GraphicsPipeline;
	friend class Semaphore;
	friend class Fence;
	friend class Buffer;
	friend class BufferResource;
	friend class BufferWithResource;

	friend bool setupDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT* callback);
	
	friend bool setdownDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT callback);
};

class WSISurface : public BaseObject {
public:
	// ����������
	// @param[in] surface:	Ҫ�ͽ������󶨵�surface
	// @param[in] cmdQueue: �����ύ��������Ķ���
	// @return ���ش����õĽ�����
	SwapChain* createSwapChain(SwapchainCreateInfo* info, CommandQueue* cmdQueue);

	// ��ѯ����֧�ֵ�����
	// @param[in]	surface: ����ѯ�ı���
	// @param[out]	capabilities ���ز�ѯ��������
	void querySurfaceCapabilities(VkSurfaceCapabilitiesKHR* capabilities);

	// ��ѯ����֧�ֵĸ�ʽ
	// @param[in] surface: ����ѯ�ı���
	// @return	���ز�ѯ�������и�ʽ
	EVECTOR<VkSurfaceFormatKHR> querySurfaceFormats();

	// ��ѯ����֧�ֵĳ��ַ�ʽ
	// @param[in] surface: ����ѯ�ı���
	// @return ���ز�ѯ��������֧�ֵĳ��ַ�ʽ
	EVECTOR<VkPresentModeKHR> queryPresentMode();

	~WSISurface() {
		vkDestroySurfaceKHR(EINSTANCE, surface, EALLOC_CB);
	}
	
private:
	WSISurface(VkSurfaceKHR surface, VkPhysicalDevice phyDevice, VkDevice device) :
		surface(surface), phyDev(phyDevice), device(device) {}

	VkSurfaceKHR	 surface;
	VkPhysicalDevice phyDev;
	VkDevice		 device;

	friend class Device;
};

class Device : public BaseObject {
public:
	WSISurface* createSurface(NativeSurface* surface);

	// ��ѯ�����Ƿ���Գ���
	bool checkCommandQueuePrensent(WSISurface* surface, CommandObjectType cmdQueueType);

	// ������Դ
	// @param[in] size:	Ҫ��������Դ��С
	// @param[in] type:	Ҫ��������Դ����
	// @param[in] vkMemoryType: Ҫ��������Դvkԭ�����ͣ�ֻ�е�typeΪCustomHeapʱ����Ч
	// @return ���ش�������Դ����
	Resource* createResource(uint64_t size, DeviceMemoryType type, uint32_t vkMemoryType = 0);

	// ��������������ͼ����ͼ
	// @param[in] img:		��������ͼ�Ľ���������
	// @param[in] format:	�����ʽ
	// @param[in] mapping:	��ɫͨ��ӳ�䣬Ĭ��Ϊnullptr����ʾĬ������ӳ��
	// @return ���ش�������ͼ
	ImageView* createSwapchainImageView(Image* img, VkFormat format, VkComponentMapping* mapping = nullptr);
				
	// ����������
	// @param[in] queueType:	Ҫ��õ������������
	// @param[in] queueIndex:	�����ڶ�����������
	// @return ���ػ�õ��������
	CommandQueue* getCommandQueue(CommandObjectType queueType, uint32_t queueIndex = 0);

	// ���������
	// @param[in] poolType: ���������
	// @param[in] flags:	����ص���Ϊ���Ա�ʾ
	// @return	���ش����������
	CommandAllocator* createCommandAllocator(CommandObjectType poolType, CommandAllocatorFlag flags = CommandAllocatorFlag::Default);

	// ������ɫ���ļ�
	// @param[in] sourceCodePath: ��ɫ���ļ�·��
	// @return ������ɫ���ļ�����
	ShaderFile* readShaderFile(const char* sourceCodePath);

	// ������ˮ�߲���
	// @param[in] descSetLayouts:
	// @param[in] pushConstantRange:
	// @param[in] flag:
	// @return ������ˮ�߲���
	PipelineLayout* createPipelineLayout(EVECTOR<VkDescriptorSetLayout>* descSetLayouts = nullptr, EVECTOR<VkPushConstantRange>* pushConstantRange = nullptr, VkPipelineLayoutCreateFlags flag = VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT);

	// ������Ⱦ���̶���
	// @param[in] attachments: ��Ⱦ���̰󶨵ĸ�����Ϣ
	// @param[in] subpasses: ��������Ϣ
	// @param[in] dependecies: ������������Ϣ
	// @return ������Ⱦ���̶���
	RenderPass* createRenderPass(EVECTOR<VkAttachmentDescription>* attachments, EVECTOR<VkSubpassDescription>* subpasses, EVECTOR<VkSubpassDependency>* dependecies);

	// ����ͼ����ˮ�߶���
	// @param[in] createInfos: ���д�������ͼ����ˮ�߶���������Ϣ
	// @return �������д�����ͼ����ˮ�߶���
	GraphicsPipeline* createGraphicsPipeline(PipeCreateInfo& createInfos, EVECTOR<ShaderStage>& shaderStages, PipelineLayout* layout, RenderPass* renderPass, uint32_t subpassIndex = 0);

	// ����֡����
	// @param[in] renderPass: ֡����󶨵�RenderPass
	// @param[in] attachments: ֡����󶨵ĸ���
	// @param[in] width: ֡�����
	// @param[in] height: ֡�����
	// @param[in] layers: ʹ�õ�ͼ�������һ��Ϊ1
	// @return ���ش�����֡����
	FrameBuffer* createFrameBuffer(RenderPass* renderPass, EVECTOR<ImageView*>& attachments, uint32_t width, uint32_t height, uint32_t layers = 1);

	// �����ź���
	// @param[in] flags: 
	// @return ���ش������ź���
	Semaphore* createSemaphore(VkSemaphoreCreateFlags flags = 0);

	// ����Fence
	Fence* createFence(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);

	// �ȴ�Fence
	void waitForFences(EVECTOR<Fence*>& fences, bool waitAll = true, uint64_t timeout = UINT64_MAX);

	// �ȴ���������
	void waitIdle() { CHECK_VK(vkDeviceWaitIdle(device)); }

	// ��ѯԭʼ�豸Vulkan Device memory����
	VkPhysicalDeviceMemoryProperties queryVkDeviceMemory();

	// ��������Buffer
	// @param[in] size: Buffer��С
	// @param[in] offset: Buffer��ʼλ��Ԥ����Resource�ϵ�ƫ������ʵ��ƫ������Ҫ�پ����ڴ���룬���ʵ��ƫ�������ܴ���offset
	// @param[in] sharedCmdType: ���Թ���Buffer������������ͣ�Ĭ��Ϊnullptr����ʾ������
	// @return ���ش�����Buffer��������ʧ�ܷ���nullptr
	Buffer* createVertexBuffer(DeviceMemoryType type, uint64_t size, EVECTOR<CommandObjectType>* sharedCmdType = nullptr, uint32_t vkMemoryType = 0) {
		return createBuffer(type, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, sharedCmdType, vkMemoryType);
	}

	// ����Դ�ϴ���һ��Buffer
	// @param[in] type: 
	// @param[in] usage: Bufferʹ������
	// @param[in] size: Buffer��С
	// @param[in] sharedCmdType: ���Թ���Buffer������������ͣ�Ĭ��Ϊnullptr����ʾ������
	// @param[in] vkMemoryType:
	// @return ���ش�����Buffer��������ʧ�ܷ���nullptr
	Buffer* createBuffer(DeviceMemoryType type, VkBufferUsageFlags usage, uint64_t size, EVECTOR<CommandObjectType>* sharedCmdType = nullptr, uint32_t vkMemoryType = 0);

	~Device() { vkDestroyDevice(device, EALLOC_CB); }

private:

	// ��ѯVulkanԭʼ�ڴ����ԣ���ΪDeviceMemoryType�ĳ�Աָ��ԭʼ�ڴ�����
	void getMemoryType();

private:
	Device(VkDevice device, VkPhysicalDevice phyDevice);

	VkPhysicalDevice				 phyDev;
	VkDevice						 device;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	// ��¼��ͬ����������ж�Ӧ�Ķ�������������������岻���ڣ�����-1��ʾ
	EVECTOR<int>	  cmdQueueFamilyIndexList;

	// ��¼��ͬ���ͶѶ�Ӧ��vk�豸�ڴ�����
	EVECTOR<uint32_t> memoryType;

	// ��¼��ͬ���Ͷѵ�vk�豸�ڴ���Ϣ
	struct HeapInfo {
		uint64_t			  size;
		VkMemoryHeapFlags	  heapFlags;
		VkMemoryPropertyFlags propertyFlags;
	};
	EVECTOR<HeapInfo> heapInfos;

	friend class Factory;
	friend class CommandAllocator;
	friend class CommandList;
	friend class SwapChain;
	friend class Resource;
	friend class Image;
	friend class ImageView;
	friend class ShaderFile;
	friend class RenderPass;
	friend class PipelineLayout;
	friend class GraphicsPipeline;
	friend class FrameBuffer;
	friend class Semaphore;
};

class SwapChain : public BaseObject {
public:
	// ��ý���������������
	uint32_t getBuffersCount() { return buffersCount; }

	// ��ý���������
	Image* getBuffer(uint32_t index);

	// ��ý�����ͼ���ʽ
	VkFormat getFormat() { return format; }

	// �ӽ�������ȡ����Ⱦͼ������
	uint32_t getNextImageIndex(Semaphore* semaphore, Fence* fence = nullptr, uint64_t timeout = UINT64_MAX);

	// 
	void present(uint32_t imageIndex, EVECTOR<Semaphore*>* semaphores = nullptr);

	~SwapChain();
private:
	SwapChain(VkSwapchainKHR swapchain, uint32_t buffersCount, VkFormat format, VkQueue	queue, VkDevice device) :
		swapchain(swapchain), buffersCount(buffersCount), format(format), queue(queue), device(device) {}

	uint32_t			buffersCount;
	VkSwapchainKHR		swapchain;
	VkFormat			format;
	VkQueue				queue;
	VkDevice			device;
	EVECTOR<VkImage>	images;

	friend class WSISurface;
};

class FrameBuffer : public BaseObject {
public:
	~FrameBuffer() {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

private:
	FrameBuffer(VkFramebuffer framebuffer, VkDevice device) :
		framebuffer(framebuffer), device(device) {}

	VkFramebuffer framebuffer;

	VkDevice device;

	friend class Device;
	friend class CommandList;
};

class CommandQueue : public BaseObject {
public:
	// ִ����Ⱦ����
	// @param[in] submitInfos: �ύ��Ϣ����
	// @param[in] fence: ����ͬ��ָ���ȫ��ִ�н����󷢳��ź�
	void execute(EVECTOR<ExecuteInfo>& submitInfos, Fence* fence = nullptr);

private:
	CommandQueue(VkQueue queue) : queue(queue) {}

	VkQueue queue = VK_NULL_HANDLE;

	friend class Device;
	friend class WSISurface;
};

class CommandAllocator : public BaseObject {
public:

	// �����������
	GraphicsCommandList* createGraphicsCommandList(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	~CommandAllocator();

private:
	CommandAllocator(VkCommandPool CommandAllocator, VkDevice device, CommandObjectType type) :
		pool(CommandAllocator), device(device), type(type) {}

	VkCommandPool pool;
	VkDevice	  device;

	CommandObjectType type;

	friend class Device;
};

class CommandList : public BaseObject {
public:
	void beginRecord(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);

	void beginRenderPass(RenderPass* renderPass, FrameBuffer* framebuffer, VkRect2D& renderArea, EVECTOR<VkClearValue>& clearValues, VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE);

	void endRenderPass() { vkCmdEndRenderPass(cmdBuffer); }

	void endRecord() { CHECK_VK(vkEndCommandBuffer(cmdBuffer)); }

protected:
	CommandList(VkCommandBuffer cmdBuffer) : cmdBuffer(cmdBuffer) {}

	VkCommandBuffer cmdBuffer;

	friend class CommandQueue;
};

class TransferCommandList : public CommandList {
protected:
	TransferCommandList(VkCommandBuffer cmdBuffer) : CommandList(cmdBuffer) {}
};

class ComputeCommandList : public TransferCommandList {
protected:
	ComputeCommandList(VkCommandBuffer cmdBuffer) : TransferCommandList(cmdBuffer) {}
};

class GraphicsCommandList : public ComputeCommandList {
public:
	void setGraphicsPipeline(GraphicsPipeline* pipeline);

	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
		vkCmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}

protected:
	GraphicsCommandList(VkCommandBuffer cmdBuffer) : ComputeCommandList(cmdBuffer) {}

	friend class CommandAllocator;
};

class Resource : public BaseObject {
public:
	// ӳ��Resource�ڵ�һƬ�������ڴ棬��sizeΪĬ��ֵ0ʱ��ӳ������buffer
	// @param[in] size: ��ʾ��ӳ�������С��0��ʾӳ������Resource
	// @param[in] offset: ��ʾ��ӳ��������ʼλ�����Resourceʵ����ʼλ�õ�ƫ����
	// @return �ɹ�����host�ɷ��ʵ��ڴ�ָ�룬ʧ�ܷ���nullptr
	void* map(uint64_t size = 0, uint64_t offset = 0);

	bool unmap();

	// ��Դ���ݸ��Ƶ�Resourceָ��������������
	// @param[in] srcData: ����Դ����
	// @param[in] size: �����Ƶ����ݴ�С����λ�ֽڣ�0��ʾ��������Resource
	// @param[in] offset: �����Ƶ�������ʼλ��ƫ����
	// @return �ɹ�����true����֮false
	bool copy(void* srcData, uint64_t size = 0, uint64_t offset = 0);
	
	// ��������Buffer
	// @param[in] size: Buffer��С
	// @param[in] offset: Buffer��ʼλ��Ԥ����Resource�ϵ�ƫ������ʵ��ƫ������Ҫ�پ����ڴ���룬���ʵ��ƫ�������ܴ���offset
	// @param[in] sharedCmdType: ���Թ���Buffer������������ͣ�Ĭ��Ϊnullptr����ʾ������
	// @return ���ش�����Buffer��������ʧ�ܷ���nullptr
	Buffer* createVertexBuffer(uint64_t size, uint64_t offset = 0, EVECTOR<CommandObjectType>* sharedCmdType = nullptr) {
		return createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, offset, sharedCmdType);
	}

	// ����Դ�ϴ���һ��Buffer
	// @param[in] usage: Bufferʹ������
	// @param[in] size: Buffer��С
	// @param[in] offset: Buffer��ʼλ��Ԥ����Resource�ϵ�ƫ������ʵ��ƫ������Ҫ�پ����ڴ���룬���ʵ��ƫ�������ܴ���offset
	// @param[in] sharedCmdType: ���Թ���Buffer������������ͣ�Ĭ��Ϊnullptr����ʾ������
	// @return ���ش�����Buffer��������ʧ�ܷ���nullptr
	Buffer* createBuffer(VkBufferUsageFlags usage, uint64_t size, uint64_t offset = 0, EVECTOR<CommandObjectType>* sharedCmdType = nullptr);
	
	~Resource() { vkFreeMemory(vkDevice, memory, EALLOC_CB); }

private:
	Resource(VkDeviceMemory memory, uint64_t size, DeviceMemoryType type, VkMemoryHeapFlags heapFlags, VkMemoryPropertyFlags propFlags,VkDevice vkDevice, Device* device)
		: memory(memory), size(size), type(type), heapFlags(heapFlags), propertyFlags(propFlags), vkDevice(vkDevice), device(device) {}

	VkDeviceMemory		  memory;
	uint64_t			  size;
	DeviceMemoryType	  type;
	VkMemoryHeapFlags	  heapFlags;
	VkMemoryPropertyFlags propertyFlags;
	Device*				  device;
	VkDevice			  vkDevice;
	bool				  isMapped = false;

	friend class Device;
	friend class BufferResource;
};

class Buffer : public BaseObject {
public:
	// ӳ��Buffer�����ڵ�һƬ�����ڴ棬��sizeΪĬ��ֵ0ʱ��ӳ������buffer
	// ���������Դ����host���ɼ����ͣ�Ĭ�϶ѣ�����mapʧ��
	// @param[in] size: ��ʾ��ӳ�������С����sizeΪĬ��ֵ0ʱ��ӳ������buffer
	// @param[in] offset: ��ʾ��ӳ��������ʼλ�����Bufferʵ����ʼλ�õ�ƫ����
	// @return �ɹ�����host�ɷ��ʵ��ڴ�ָ�룬ʧ�ܷ���nullptr
	virtual void* map(uint64_t size = 0, uint64_t offset = 0) = 0;

	// �ͷ�ӳ��
	virtual bool unmap() = 0;

	// ��Դ���ݸ��Ƶ�Bufferָ��������������
	// @param[in] srcData: ����Դ����
	// @param[in] size: �����Ƶ����ݴ�С����λ�ֽڣ�0��ʾ��������Buffer
	// @param[in] offset: �����Ƶ�������ʼλ��ƫ����
	// @return �ɹ�����true����֮false
	virtual bool copy(void* srcData, uint64_t size = 0, uint64_t offset = 0) = 0;

	// ���Buffer��ʼλ����Resource�ϵ�λ��
	virtual uint64_t getStartOffset() { return realOffset; }

	// ���Bufferʵ�ʴ�С
	virtual uint64_t getSize() { return realSize; }

	// ���Buffer�ڴ����λ�����Resource��λ��
	virtual uint64_t getEndOffset() { return realOffset + realSize; }

	virtual ~Buffer() {}

protected:
	Buffer(VkDevice device, uint32_t size, uint32_t realSize, uint32_t offset, uint32_t realOffset) :
		device(device), size(size), realSize(realSize), offset(offset), realOffset(realOffset) {}

	VkDevice  device;
	uint64_t  size;					// ϣ������Ŀռ��С
	uint64_t  realSize;				// ʵ����resource�Ϸ���Ŀռ��С
	uint64_t  offset;				// ϣ����resource�ϵ�ƫ��
	uint64_t  realOffset;			// ���������ʵ����resource�ϵ�ƫ��
};

class BufferResource : public Buffer {
public:
	virtual void* map(uint64_t size = 0, uint64_t offset = 0);

	virtual bool unmap();

	virtual bool copy(void* srcData, uint64_t size = 0, uint64_t offset = 0);

	virtual uint64_t getStartOffset() { return realOffset; }

	virtual uint64_t getSize() { return realSize; }

	virtual uint64_t getEndOffset() { return realOffset + realSize; }

	virtual ~BufferResource() { vkDestroyBuffer(device, buffer, EALLOC_CB); }

private:
	BufferResource(VkBuffer buffer, VkDevice device, uint32_t size, uint32_t realSize, uint32_t offset, uint32_t realOffset, Resource* resource) :
		buffer(buffer), resource(resource), Buffer(device, size, realSize, offset, realOffset) {}

	VkBuffer  buffer;
	Resource* resource;

	friend class Resource;
};

class BufferWithResource : public Buffer {
public:
	virtual void* map(uint64_t size = 0, uint64_t offset = 0);

	virtual bool unmap();

	virtual bool copy(void* srcData, uint64_t size = 0, uint64_t offset = 0);

	virtual uint64_t getStartOffset() { return 0; }

	virtual uint64_t getEndOffset() { return realSize; }

	virtual ~BufferWithResource() {
		vkDestroyBuffer(device, buffer, EALLOC_CB);
		vkFreeMemory(device, memory, EALLOC_CB);
	}

private:
	BufferWithResource(
		VkDeviceMemory memory, VkBuffer buffer, DeviceMemoryType type, VkMemoryPropertyFlags propertyFlags, VkMemoryHeapFlags heapFlags,
		VkDevice device, uint32_t size, uint32_t realSize) :
		memory(memory), buffer(buffer), type(type), heapFlags(heapFlags), propertyFlags(propertyFlags),
		Buffer(device, size, realSize, 0, 0) {}

	VkBuffer			  buffer;
	VkDeviceMemory		  memory;
	DeviceMemoryType	  type;
	VkMemoryHeapFlags	  heapFlags;
	VkMemoryPropertyFlags propertyFlags;
	bool				  isMapped = false;

	friend class Device;
};

class Image : public BaseObject {
public:

	virtual ~Image() = 0;

protected:
	Image(VkImage img) : image(img) {}

	VkImage image;

	friend class Device;
};

class ImageView : public BaseObject {
public:
	~ImageView() {
		vkDestroyImageView(device, imgView, EALLOC_CB);
	}
private:
	ImageView(VkImageView imgView) : 
		imgView(imgView), device(device) {}

	VkImageView imgView;

	VkDevice	device;

	friend class Device;
};

class ShaderFile : public BaseObject {
public:
	~ShaderFile() { vkDestroyShaderModule(device, shaderModule, EALLOC_CB); }

private:
	ShaderFile(VkShaderModule shaderModule, VkDevice device) : 
		shaderModule(shaderModule), device(device) {}

	VkShaderModule shaderModule;
	VkDevice	   device;

	friend class Device;
};

class PipelineLayout : public BaseObject {
public:
	~PipelineLayout() { vkDestroyPipelineLayout(device, pipelineLayout, EALLOC_CB); }

private:
	PipelineLayout(VkPipelineLayout pipelineLayout, VkDevice device) : 
		pipelineLayout(pipelineLayout), device(device) {}

	VkPipelineLayout pipelineLayout;
	VkDevice		 device;

	friend class Device;
};

class RenderPass : public BaseObject {
public:
	~RenderPass() { vkDestroyRenderPass(device, renderPass, EALLOC_CB); }

private:
	RenderPass(VkRenderPass renderPass, VkDevice device) : 
		renderPass(renderPass), device(device) {}

	VkRenderPass renderPass;
	VkDevice	 device;

	friend class Device;
	friend class CommandList;
};

class Pipeline : public BaseObject {
public:
	virtual ~Pipeline() = 0;

protected:
	Pipeline(VkPipeline pipeline, VkDevice device) :
		pipeline(pipeline), device(device) {}

	VkPipeline pipeline;
	VkDevice   device;

	friend class Device;
};

class GraphicsPipeline : public Pipeline {
public:
	virtual ~GraphicsPipeline() { vkDestroyPipeline(device, pipeline, EALLOC_CB); }

private:
	GraphicsPipeline(VkPipeline pipeline) : Pipeline(pipeline, device) {}

	VkPipeline getVkPipeline();

	friend class Device;
	friend class GraphicsCommandList;
};

class Semaphore : public BaseObject {
public:
	~Semaphore() { vkDestroySemaphore(device, semaphore, EALLOC_CB); }

private:
	Semaphore(VkSemaphore semaphore) : 
		semaphore(semaphore), device(device) {}

	VkSemaphore semaphore;
	VkDevice    device;

	friend class Device;
	friend class CommandQueue;
	friend class SwapChain;
};

class Fence : public BaseObject {
public:
	~Fence() { vkDestroyFence(device, fence, EALLOC_CB); }

private:
	Fence(VkFence fence, VkDevice device) :
		fence(fence), device(device) {}

	VkFence  fence;
	VkDevice device;

	friend class Device;
	friend class CommandQueue;
	friend class SwapChain;
};

EINLINE SwapChain::~SwapChain() {
	vkDestroySwapchainKHR(device, swapchain, EALLOC_CB);
}

EINLINE CommandAllocator::~CommandAllocator() {
	
	vkDestroyCommandPool(device, pool, EALLOC_CB);
}

EINLINE Pipeline::~Pipeline() {}

EINLINE void GraphicsCommandList::setGraphicsPipeline(GraphicsPipeline* pipeline) {
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getVkPipeline());
}

EINLINE Image::~Image() {}

#endif