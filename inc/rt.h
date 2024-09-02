#ifndef RT_H
#define RT_H

#ifdef VKMLRT_EXPORTS
#define VKMLRT_API __declspec(dllexport)
#else
#define VKMLRT_API __declspec(dllimport)
#endif


#include <memory>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <deque>

#define VMA_VULKAN_VERSION 103000
#define VK_NO_PROTOTYPES
#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "volk.h"
#include "vk_mem_alloc.h"

namespace vkmlrt {


    typedef struct pool
    {
        VkDescriptorPool currentPool = { VK_NULL_HANDLE };
        std::vector<VkDescriptorPool> usedPools;
        std::vector<VkDescriptorPool> readyPools;
    } pool;

    typedef struct descriptor {
        std::deque<VkDescriptorImageInfo> imageInfos;
        std::vector<VkWriteDescriptorSet> writes;
    } descriptor;


    typedef struct vkmlrt_allocator
    {
        VmaAllocator a = VK_NULL_HANDLE;			
        pool p;
    } vkmlrt_allocator;

    typedef struct vkmlrt_compute_pgrm {
        VkShaderModule module;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
		VkDescriptorSetLayout layout;
		VkShaderModule shaderModule;
        VkPipeline pipeline;
        descriptor d;
    } vkmlrt_compute_pgrm;


    typedef struct  vkmlrt_device
    {
        VkPhysicalDevice pd = VK_NULL_HANDLE;
        VkDevice ld = VK_NULL_HANDLE;
        vkmlrt_allocator a;
        std::vector<VkQueue> q;

        std::vector<VmaPool> pools;
        VkCommandPool computePool = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        
        std::vector<vkmlrt_compute_pgrm> computePrograms;
		std::unordered_map<VkBuffer, VmaAllocation> buffers;
		std::unordered_map<VkImage, VmaAllocation> images;
    } vkmlrt_device;

    void init(std::vector<vkmlrt_device>& devices);

    VkDescriptorPool getPool(vkmlrt_device& device);

    void write_image(vkmlrt_compute_pgrm& pgrm, int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkShaderStageFlagBits stageFlag = VK_SHADER_STAGE_COMPUTE_BIT);

    void write_buffer(vkmlrt_device& device, vkmlrt_compute_pgrm& pgrm, int binding, VkBuffer buffer, VkShaderStageFlagBits stageFlag = VK_SHADER_STAGE_COMPUTE_BIT);

	void write_shader_pgrm(vkmlrt_device& device, vkmlrt_compute_pgrm& pgrm, const unsigned char* code, size_t size);

    void write_compute_program(vkmlrt_device& device, vkmlrt_compute_pgrm& pgrm);


   
   

    void destroy(std::vector<vkmlrt_device>& devices)
    {
        auto instance = volkGetLoadedInstance();
        for (auto& dev : devices)
        {            
			vmaDestroyAllocator(dev.a.a);
            vkDestroyDevice(dev.ld, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
    }

    auto allocateBuffer(vkmlrt_device& device, VkDeviceSize size, VkBufferUsageFlags usage)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VkBuffer buffer;
        VmaAllocation alloc;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if (bufferInfo.usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        {
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        }
        if (bufferInfo.usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        {
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }
        if (bufferInfo.usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)
        {
            allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        }
		if (bufferInfo.usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		{
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
        uint32_t memTypeIndex;
		auto res = vmaFindMemoryTypeIndexForBufferInfo(device.a.a, &bufferInfo, &allocInfo, &memTypeIndex);
		if (res != VK_SUCCESS)
		{
			printf("Failed to find memory type index\n");
		}
        if (memTypeIndex + 1llu > device.pools.size())
        {
            VmaPoolCreateInfo poolInfo = {};
            poolInfo.memoryTypeIndex = memTypeIndex;
            poolInfo.blockSize = std::min<size_t>(2* 1024llu * 1024 * 1024, std::max<size_t>(size * 2, 1024llu * 1024));
            poolInfo.flags = VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT;
            poolInfo.priority = 0.0f;
            device.pools.resize(memTypeIndex + 1llu);
			/*auto res = vmaCreatePool(device.allocator, &poolInfo, &device.pools[memTypeIndex]);
            if (res != VK_SUCCESS)
            {
                printf("Failed to create pool\n");
            }*/
        }
		
        //allocInfo.pool = device.pools[memTypeIndex];     

		auto res2 = vmaCreateBuffer(device.a.a, &bufferInfo, &allocInfo, &buffer, &alloc, nullptr);
        if (res2 != VK_SUCCESS)
        {
            printf("Failed to create Buffer\n");
        }

		device.buffers[buffer] = alloc;
        return buffer;
    }
   
} // namespace vkrt

#endif // RT_H