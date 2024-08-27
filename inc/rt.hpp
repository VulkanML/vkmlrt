#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "dev.hpp"


class rt
{
    rt()
    {
        vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine", VK_MAKE_VERSION(1, 0, 0),
                                    VK_API_VERSION_1_1);
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



