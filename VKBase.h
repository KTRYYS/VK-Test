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

// ELOGx是封装成专用LOG，若使用性能模式，则此LOG关闭
#define ElogI(format, ...)
#define ElogW(format, ...)
#define ElogE(format, ...)

// 有三种编译模式，分别是性能模式 MODE_PERFORMANCE、普通模式：MODE_NORMAL、调试模式 MODE_DEBUG
// 调试模式会打开vulkan调试层，性能模式会关闭几乎所有log（主要是Elog)
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
// 检查返回类型为 VkResult 的vk函数返回值
#define CHECK_VK(func) { VkResult ret = func; if (ret != VK_SUCCESS) { logI("Failed in File: %s Line: %d\n", THE_FILE, THE_LINE); checkErrCode(ret); assert(0); } }

#define CHECK_INST assert(EINSTANCE != VK_NULL_HANDLE);

// 启用VK_EXT_debug_tuils校验层扩展并注册回调函数
bool setupDebugExt(VkInstance instance, VkDebugUtilsMessengerEXT* callback);

// 注销回调函数
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

// 自动将一个指针转换为智能指针
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

// 描述交换链创建信息
struct SwapchainCreateInfo {
	uint32_t						width;														// 窗口宽
	uint32_t						height;														// 窗口高
	uint32_t						buffersCount = 3;											// 交换链缓冲数量
	VkSurfaceFormatKHR				format;														// 交换链图像格式
	VkPresentModeKHR				presentMode = VK_PRESENT_MODE_MAILBOX_KHR;					// 交换链呈现模式
	VkSurfaceTransformFlagBitsKHR	transMode = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;			// 图像呈现前变化模式
	VkCompositeAlphaFlagBitsKHR		alphaUsage = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;				// alpha通道用处
	SwapChain*						oldSwapchain = nullptr;								// 老交换链
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

	// 查看设备数量
	uint64_t getAdapterCount() { return physicalDevices.size(); }

	// 查询物理设备信息
	// @param[in] index: 待查询设备的索引
	// @param[out] info: 用于返回设备信息
	// @return 成功返回true，反之false
	bool getAdapterInfo(uint32_t phyDevIdx, VkPhysicalDeviceProperties* info);

	//  查询物理设备能力
	// @param[in] index: 待查询设备的索引
	// @param[out] capacities: 用于返回设备能力
	// @return 成功返回true，反之false
	bool getAdapterCapacities(uint32_t phyDevIdx, VkPhysicalDeviceFeatures* capacities);

	// 创建逻辑设备，包含图形、计算、传输、视频解码队列（如果以上几者都有的话），并且优先级都为1
	// @param[in] phyDevIdx: 和逻辑设备绑定的物理设备索引
	// @return 返回创建的逻辑设备
	Device* createDevice(uint32_t phyDevIdx);

	virtual ~Factory();

private:
	Factory(EngineInitInfo* info, VkAllocationCallbacks* allocCB);

	// 创建Vulkan实例
	void createVkInstance();

	// 获得全部物理设备信息
	void getAllPhysicalDevice();

	// 获得物理设备队列类型信息
	uint32_t getPhyDevCmdQueueTypeInfo(VkPhysicalDevice physicalDevice, EVECTOR<VkQueueFamilyProperties>& queueFamiliesProp, EVECTOR<int>& cmdQueueFamilyIndexList);

	// 销毁实例
	void delInstance();

private:
	static Factory*				  myself;
	static VkInstance			  instance;
	static VkAllocationCallbacks* allocCB;
								  
	std::string					  appName;
	std::string					  engineName;
	uint32_t					  apiVersion;
								  
	VkDebugUtilsMessengerEXT	  dbgUtilMsgExt;
								     
	// 查询到的所有物理设备		   
	EVECTOR<VkPhysicalDevice>	  physicalDevices;

	// 记录不同类型设备内存对应的vk设备内存索引
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
	// 创建交换链
	// @param[in] surface:	要和交换链绑定的surface
	// @param[in] cmdQueue: 用于提交呈现命令的队列
	// @return 返回创建好的交换链
	SwapChain* createSwapChain(SwapchainCreateInfo* info, CommandQueue* cmdQueue);

	// 查询表面支持的能力
	// @param[in]	surface: 被查询的表面
	// @param[out]	capabilities 返回查询到的能力
	void querySurfaceCapabilities(VkSurfaceCapabilitiesKHR* capabilities);

	// 查询表面支持的格式
	// @param[in] surface: 被查询的表面
	// @return	返回查询到的所有格式
	EVECTOR<VkSurfaceFormatKHR> querySurfaceFormats();

	// 查询表面支持的呈现方式
	// @param[in] surface: 被查询的表面
	// @return 返回查询到的所有支持的呈现方式
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

	// 查询队列是否可以呈现
	bool checkCommandQueuePrensent(WSISurface* surface, CommandObjectType cmdQueueType);

	// 创建资源
	// @param[in] size:	要创建的资源大小
	// @param[in] type:	要创建的资源类型
	// @param[in] vkMemoryType: 要创建的资源vk原生类型，只有当type为CustomHeap时才生效
	// @return 返回创建的资源对象
	Resource* createResource(uint64_t size, DeviceMemoryType type, uint32_t vkMemoryType = 0);

	// 创建交换链缓冲图像视图
	// @param[in] img:		被创建视图的交换链缓冲
	// @param[in] format:	缓冲格式
	// @param[in] mapping:	颜色通道映射，默认为nullptr，表示默认正常映射
	// @return 返回创建的视图
	ImageView* createSwapchainImageView(Image* img, VkFormat format, VkComponentMapping* mapping = nullptr);
				
	// 获得命令队列
	// @param[in] queueType:	要获得的命令队列类型
	// @param[in] queueIndex:	队列在队列族中索引
	// @return 返回获得的命令队列
	CommandQueue* getCommandQueue(CommandObjectType queueType, uint32_t queueIndex = 0);

	// 创建命令池
	// @param[in] poolType: 命令池类型
	// @param[in] flags:	命令池的行为特性表示
	// @return	返回创建的命令池
	CommandAllocator* createCommandAllocator(CommandObjectType poolType, CommandAllocatorFlag flags = CommandAllocatorFlag::Default);

	// 加载着色器文件
	// @param[in] sourceCodePath: 着色器文件路径
	// @return 返回着色器文件对象
	ShaderFile* readShaderFile(const char* sourceCodePath);

	// 创建流水线布局
	// @param[in] descSetLayouts:
	// @param[in] pushConstantRange:
	// @param[in] flag:
	// @return 返回流水线布局
	PipelineLayout* createPipelineLayout(EVECTOR<VkDescriptorSetLayout>* descSetLayouts = nullptr, EVECTOR<VkPushConstantRange>* pushConstantRange = nullptr, VkPipelineLayoutCreateFlags flag = VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT);

	// 创建渲染流程对象
	// @param[in] attachments: 渲染流程绑定的附着信息
	// @param[in] subpasses: 子流程信息
	// @param[in] dependecies: 子流程依赖信息
	// @return 返回渲染流程对象
	RenderPass* createRenderPass(EVECTOR<VkAttachmentDescription>* attachments, EVECTOR<VkSubpassDescription>* subpasses, EVECTOR<VkSubpassDependency>* dependecies);

	// 创建图形流水线对象
	// @param[in] createInfos: 所有待创建的图形流水线对象描述信息
	// @return 返回所有创建的图形流水线对象
	GraphicsPipeline* createGraphicsPipeline(PipeCreateInfo& createInfos, EVECTOR<ShaderStage>& shaderStages, PipelineLayout* layout, RenderPass* renderPass, uint32_t subpassIndex = 0);

	// 创建帧缓冲
	// @param[in] renderPass: 帧缓冲绑定的RenderPass
	// @param[in] attachments: 帧缓冲绑定的附件
	// @param[in] width: 帧缓冲宽
	// @param[in] height: 帧缓冲高
	// @param[in] layers: 使用的图像层数，一般为1
	// @return 返回创建的帧缓冲
	FrameBuffer* createFrameBuffer(RenderPass* renderPass, EVECTOR<ImageView*>& attachments, uint32_t width, uint32_t height, uint32_t layers = 1);

	// 创建信号量
	// @param[in] flags: 
	// @return 返回创建的信号量
	Semaphore* createSemaphore(VkSemaphoreCreateFlags flags = 0);

	// 创建Fence
	Fence* createFence(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);

	// 等待Fence
	void waitForFences(EVECTOR<Fence*>& fences, bool waitAll = true, uint64_t timeout = UINT64_MAX);

	// 等待操作结束
	void waitIdle() { CHECK_VK(vkDeviceWaitIdle(device)); }

	// 查询原始设备Vulkan Device memory属性
	VkPhysicalDeviceMemoryProperties queryVkDeviceMemory();

	// 创建顶点Buffer
	// @param[in] size: Buffer大小
	// @param[in] offset: Buffer起始位置预计在Resource上的偏移量，实际偏移量需要再经过内存对齐，因此实际偏移量可能大于offset
	// @param[in] sharedCmdType: 可以共享Buffer的命令队列类型，默认为nullptr，表示不共享
	// @return 返回创建的Buffer，若创建失败返回nullptr
	Buffer* createVertexBuffer(DeviceMemoryType type, uint64_t size, EVECTOR<CommandObjectType>* sharedCmdType = nullptr, uint32_t vkMemoryType = 0) {
		return createBuffer(type, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, sharedCmdType, vkMemoryType);
	}

	// 在资源上创建一块Buffer
	// @param[in] type: 
	// @param[in] usage: Buffer使用类型
	// @param[in] size: Buffer大小
	// @param[in] sharedCmdType: 可以共享Buffer的命令队列类型，默认为nullptr，表示不共享
	// @param[in] vkMemoryType:
	// @return 返回创建的Buffer，若创建失败返回nullptr
	Buffer* createBuffer(DeviceMemoryType type, VkBufferUsageFlags usage, uint64_t size, EVECTOR<CommandObjectType>* sharedCmdType = nullptr, uint32_t vkMemoryType = 0);

	~Device() { vkDestroyDevice(device, EALLOC_CB); }

private:

	// 查询Vulkan原始内存特性，并为DeviceMemoryType的成员指定原始内存索引
	void getMemoryType();

private:
	Device(VkDevice device, VkPhysicalDevice phyDevice);

	VkPhysicalDevice				 phyDev;
	VkDevice						 device;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	// 记录不同类型命令队列对应的队列族索引，如果队列族不存在，则用-1表示
	EVECTOR<int>	  cmdQueueFamilyIndexList;

	// 记录不同类型堆对应的vk设备内存索引
	EVECTOR<uint32_t> memoryType;

	// 记录不同类型堆的vk设备内存信息
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
	// 获得交换链缓冲区数量
	uint32_t getBuffersCount() { return buffersCount; }

	// 获得交换链缓冲
	Image* getBuffer(uint32_t index);

	// 获得交换链图像格式
	VkFormat getFormat() { return format; }

	// 从交换链获取可渲染图像索引
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
	// 执行渲染命令
	// @param[in] submitInfos: 提交信息数组
	// @param[in] fence: 用于同步指令缓冲全部执行结束后发出信号
	void execute(EVECTOR<ExecuteInfo>& submitInfos, Fence* fence = nullptr);

private:
	CommandQueue(VkQueue queue) : queue(queue) {}

	VkQueue queue = VK_NULL_HANDLE;

	friend class Device;
	friend class WSISurface;
};

class CommandAllocator : public BaseObject {
public:

	// 创建命令队列
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
	// 映射Resource内的一片连续的内存，当size为默认值0时，映射整个buffer
	// @param[in] size: 表示待映射区域大小，0表示映射整个Resource
	// @param[in] offset: 表示待映射区域起始位置相对Resource实际起始位置的偏移量
	// @return 成功返回host可访问的内存指针，失败返回nullptr
	void* map(uint64_t size = 0, uint64_t offset = 0);

	bool unmap();

	// 将源数据复制到Resource指定的连续区域上
	// @param[in] srcData: 复制源数据
	// @param[in] size: 待复制的数据大小，单位字节，0表示复制整个Resource
	// @param[in] offset: 待复制的区域起始位置偏移量
	// @return 成功返回true，反之false
	bool copy(void* srcData, uint64_t size = 0, uint64_t offset = 0);
	
	// 创建顶点Buffer
	// @param[in] size: Buffer大小
	// @param[in] offset: Buffer起始位置预计在Resource上的偏移量，实际偏移量需要再经过内存对齐，因此实际偏移量可能大于offset
	// @param[in] sharedCmdType: 可以共享Buffer的命令队列类型，默认为nullptr，表示不共享
	// @return 返回创建的Buffer，若创建失败返回nullptr
	Buffer* createVertexBuffer(uint64_t size, uint64_t offset = 0, EVECTOR<CommandObjectType>* sharedCmdType = nullptr) {
		return createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, offset, sharedCmdType);
	}

	// 在资源上创建一块Buffer
	// @param[in] usage: Buffer使用类型
	// @param[in] size: Buffer大小
	// @param[in] offset: Buffer起始位置预计在Resource上的偏移量，实际偏移量需要再经过内存对齐，因此实际偏移量可能大于offset
	// @param[in] sharedCmdType: 可以共享Buffer的命令队列类型，默认为nullptr，表示不共享
	// @return 返回创建的Buffer，若创建失败返回nullptr
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
	// 映射Buffer区域内的一片连续内存，当size为默认值0时，映射整个buffer
	// 若分配的资源属于host不可见类型（默认堆），则map失败
	// @param[in] size: 表示待映射区域大小，当size为默认值0时，映射整个buffer
	// @param[in] offset: 表示待映射区域起始位置相对Buffer实际起始位置的偏移量
	// @return 成功返回host可访问的内存指针，失败返回nullptr
	virtual void* map(uint64_t size = 0, uint64_t offset = 0) = 0;

	// 释放映射
	virtual bool unmap() = 0;

	// 将源数据复制到Buffer指定的连续区域上
	// @param[in] srcData: 复制源数据
	// @param[in] size: 待复制的数据大小，单位字节，0表示复制整个Buffer
	// @param[in] offset: 待复制的区域起始位置偏移量
	// @return 成功返回true，反之false
	virtual bool copy(void* srcData, uint64_t size = 0, uint64_t offset = 0) = 0;

	// 获得Buffer起始位置在Resource上的位置
	virtual uint64_t getStartOffset() { return realOffset; }

	// 获得Buffer实际大小
	virtual uint64_t getSize() { return realSize; }

	// 获得Buffer内存结束位置相对Resource的位置
	virtual uint64_t getEndOffset() { return realOffset + realSize; }

	virtual ~Buffer() {}

protected:
	Buffer(VkDevice device, uint32_t size, uint32_t realSize, uint32_t offset, uint32_t realOffset) :
		device(device), size(size), realSize(realSize), offset(offset), realOffset(realOffset) {}

	VkDevice  device;
	uint64_t  size;					// 希望分配的空间大小
	uint64_t  realSize;				// 实际在resource上分配的空间大小
	uint64_t  offset;				// 希望在resource上的偏移
	uint64_t  realOffset;			// 经过对齐后实际在resource上的偏移
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