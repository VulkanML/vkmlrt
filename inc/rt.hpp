#pragma once
#define VMA_VULKAN_VERSION 1003000
#define VK_NO_PROTOTYPES
#define VOLK_IMPLEMENTATION
#include "volk.h"

#include <memory>
#include <stdio.h>
#include <vector>

#include <vulkan/vulkan.h>
#include "dev.hpp"
#include "volk.h"

class rt
{
    rt()
    {
       

        vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)

        vk::InstanceCreateInfo createInfo({}, &appInfo);
        _inst = vk::createInstance(createInfo);
        for (auto &pd : _inst.enumeratePhysicalDevices())
        {
            _devices.emplace_back(pd);
        }
    }

    rt(const rt &) = delete;
    rt &operator=(const rt &) = delete;
    static std::shared_ptr<rt> _instance;
    VkInstance _inst;
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

namespace vkrt {


struct vkrt_instance {
    VkInstance instance;
};

void init(vkrt_instance&instance) 
{
    auto res = volkInitialize();
	if (res != VK_SUCCESS) {
		printf("Failed to initialize volk\n");
	}

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    auto res = volkInitialize();

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    
    auto ins_res = vkCreateInstance(&createInfo, nullptr, &instance.instance);
	if (ins_res != VK_SUCCESS) {
		printf("Failed to create instance\n");
	}
	volkLoadInstance(instance.instance);
}

void destroy(vkrt_instance& instance) 
{
    vkDestroyInstance(instance.instance, nullptr);
}

} // namespace vkrt
