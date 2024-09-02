#pragma once

#include <memory>
#include <stdio.h>
#include <vector>

#include <vulkan/vulkan.hpp>

class rt
{
    rt()
    {
       

        vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine", VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_1);
        vk::InstanceCreateInfo createInfo({}, &appInfo);
        _inst = vk::createInstance(createInfo);
        for (auto& pd : _inst.enumeratePhysicalDevices())
        {
            _devices.emplace_back(pd);
        }
    }

    rt(const rt &) = delete;
    rt &operator=(const rt &) = delete;
    static std::shared_ptr<rt> _instance;
    vk::Instance _inst;
    vk::DebugUtilsMessengerEXT _debugUtilsMessenger;
    std::vector<dev> _devices;

  public:
    static std::shared_ptr<rt> instance();
    ~rt()
    {
        for (auto &dev_inst : _devices)
            dev_inst.destroy();
        _inst.destroy();
    }

    vk::Buffer allocateBuffer(size_t device_id, vk::DeviceSize size)
    {
        vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eStorageBuffer;
        return _devices.at(device_id).createDeviceBuffer(size, usage);
    }

    vk::Buffer allocateTransferBuffer(size_t device_id, vk::DeviceSize size)
    {
        vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
        return _devices.at(device_id).createDeviceBuffer(size, usage);
    }

    compPgrm buildFunction(size_t device_id, unsigned char shader[], size_t shaderSize, const std::vector<vk::Buffer> &buffers)
    {
        return _devices.at(device_id).define_shader_program(shader, shaderSize, buffers);
    }

    void execute(size_t device_id, compPgrm &pgrm)
    {
        _devices.at(device_id).execute(pgrm);
    }
    
};

#include <vector>
#include <vulkan/vulkan.h>

namespace vkmlrt {
#define VMA_VULKAN_VERSION 1003000
#define VK_NO_PROTOTYPES
#define VOLK_IMPLEMENTATION

#include "volk.h"
#include "vk_mem_alloc.h"


    typename struct {
        VkPhysicalDevice pd;
        VkDevice ld;
        VolkDeviceTable t;
		VmaAllocator allocator;
    } vkmlrt_device;


    auto init(std::vector<vkmlrt_device>& devices)
    {
        auto res = volkInitialize();
        if (res != VK_SUCCESS)
        {
            printf("Failed to initialize volk\n");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello ML";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "vkmlrt";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_3;
        auto res = volkInitialize();

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        VkInstance instance;
        auto ins_res = vkCreateInstance(&createInfo, nullptr, &instance);
        if (ins_res != VK_SUCCESS)
        {
            printf("Failed to create instance\n");
        }
        volkLoadInstance(instance);

        uint32_t pdCount = 0;
        vkEnumeratePhysicalDevices(instance, &pdCount, nullptr);
        std::vector<VkPhysicalDevice> pds(pdCount);
        vkEnumeratePhysicalDevices(instance, &pdCount, pds.data());

        devices.resize(pdCount);

        size_t i = 0;
        for (auto& pd : pds)
        {
            VkDevice ld;
            VkDeviceCreateInfo deviceInfo = {};
            vkCreateDevice(pd, &deviceInfo, nullptr, &ld);
            volkLoadDeviceTable(&devices[i++].t, ld);
			VmaVulkanFunctions vmaFuncs = {};
			vmaFuncs.vkGetPhysicalDeviceProperties = devices[i].t.vkGetPhysicalDeviceProperties;
			vmaFuncs.vkGetPhysicalDeviceMemoryProperties = devices[i].t.vkGetPhysicalDeviceMemoryProperties;
			vmaFuncs.vkAllocateMemory = devices[i].t.vkAllocateMemory;
			vmaFuncs.vkFreeMemory = devices[i].t.vkFreeMemory;
			vmaFuncs.vkMapMemory = devices[i].t.vkMapMemory;
			vmaFuncs.vkUnmapMemory = devices[i].t.vkUnmapMemory;
			vmaFuncs.vkFlushMappedMemoryRanges = devices[i].t.vkFlushMappedMemoryRanges;
			vmaFuncs.vkInvalidateMappedMemoryRanges = devices[i].t.vkInvalidateMappedMemoryRanges;
			vmaFuncs.vkBindBufferMemory = devices[i].t.vkBindBufferMemory;
			vmaFuncs.vkBindImageMemory = devices[i].t.vkBindImageMemory;
			vmaFuncs.vkGetBufferMemoryRequirements = devices[i].t.vkGetBufferMemoryRequirements;
			vmaFuncs.vkGetImageMemoryRequirements = devices[i].t.vkGetImageMemoryRequirements;
			vmaFuncs.vkCreateBuffer = devices[i].t.vkCreateBuffer;
			vmaFuncs.vkDestroyBuffer = devices[i].t.vkDestroyBuffer;
			vmaFuncs.vkCreateImage = devices[i].t.vkCreateImage;
			vmaFuncs.vkDestroyImage = devices[i].t.vkDestroyImage;
			vmaFuncs.vkCmdCopyBuffer = devices[i].t.vkCmdCopyBuffer;
            devices[i].pd = pd;
			devices[i].ld = ld;
			devices[i].allocator = nullptr;
			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.physicalDevice = pd;
			allocatorInfo.device = ld;
			allocatorInfo.pVulkanFunctions = &vmaFuncs;
			vmaCreateAllocator(&allocatorInfo, &devices[i].allocator);
        }


    }

    void destroy(std::vector<struct vkmlrt_device>& devices)
    {
        auto instance = volkGetLoadedInstance();
        for (auto& dev : devices)
        {
            vkDestroyDevice(dev.ld, nullptr);
			vmaDestroyAllocator(dev.allocator);
        }
        vkDestroyInstance(instance, nullptr);
    }

} // namespace vkrt
