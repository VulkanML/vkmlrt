#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "../inc/buf.h"
#include "../inc/pgrm.h"


class q
{
    vk::Queue _cmdQueue;
    vk::CommandPool _cmdPool;
    vk::CommandBuffer _primaryCmdBuffer;
    vk::CommandBuffer _secondaryCmdBuffer;
    vk::Fence _fence;

    
  public:
    q(vk::Device &dev, uint32_t qFamIdx, uint32_t queueIndex)
    {
        _fence = dev.createFence(vk::FenceCreateInfo());
        _cmdQueue = dev.getQueue(qFamIdx, queueIndex);
        _cmdPool = dev.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), qFamIdx));
        _primaryCmdBuffer =
            dev.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_cmdPool, vk::CommandBufferLevel::ePrimary, 1)).front();
        _secondaryCmdBuffer = dev.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_cmdPool, vk::CommandBufferLevel::eSecondary, 1)).front();  
    }

    void execute(vk::Device& dev, const compPgrm &pgrm)
    {
        vk::CommandBufferBeginInfo CmdBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        _primaryCmdBuffer.begin(CmdBufferBeginInfo);
        _primaryCmdBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pgrm._pipe);
        _primaryCmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pgrm._pipeLayout, 0, pgrm._descSets, {});
        _primaryCmdBuffer.dispatch(1, 1, 1);
        _primaryCmdBuffer.end();
        _cmdQueue.submit(vk::SubmitInfo(0, nullptr, nullptr, 1, &_primaryCmdBuffer), _fence);
        _cmdQueue.waitIdle();
    }

    void destroy(vk::Device &dev)
    {
        dev.freeCommandBuffers(_cmdPool, _primaryCmdBuffer);
        dev.freeCommandBuffers(_cmdPool, _secondaryCmdBuffer);
        dev.destroyCommandPool(_cmdPool);
    }

    void sync()
    {
        _cmdQueue.waitIdle();
    }

    vk::Fence getFence()
    {
        return _fence;
    }    
};



class dev
{
    vk::Device _dev;
    // std::vector<vk::Queue> _computeQ, _graphicsQ, _transferQ;
    std::vector<q> _compute_cmds, _graphics_cmds, _transfer_cmds;
    vk::PhysicalDeviceMemoryProperties _memProps;
    vk::PhysicalDeviceProperties _devProps;
    vk::PhysicalDeviceProperties2 _devProps2;
    vk::PhysicalDeviceMaintenance3Properties _devProps3;
    vk::PhysicalDeviceMaintenance4Properties _devProps4;
    std::map<vk::MemoryPropertyFlags, allocator> _allocators;
    std::unordered_map<vk::Buffer, chunk_t> _bufferChunks;
    vk::PipelineCache _pipelineCache;

    std::vector<compPgrm> _compPgrms;

    std::vector<vk::DeviceQueueCreateInfo> _defineExecQueues(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties)
    {
        std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
        for(auto i = 0; i < queueFamilyProperties.size(); ++i){
            std::vector<float> QueuePriority(queueFamilyProperties[i].queueCount, 0.0f);
            deviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), i, queueFamilyProperties[i].queueCount, &QueuePriority.back());
        }   
        return deviceQueueCreateInfos;
    }

    void _buildAllocators();
    void _buildCmds(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties);

  public:
    dev(const vk::PhysicalDevice &pd)
    {
        _memProps = pd.getMemoryProperties();

        auto heaps = _memProps.memoryTypes;
        size_t id = 0;
        size_t heapIdx = 0;

        _devProps2.pNext = &_devProps3;
        _devProps3.pNext = &_devProps4;
        pd.getProperties2(&_devProps2);
        _devProps = _devProps2.properties;        
        std::vector<uint32_t> computeQIdx, graphicsQIdx, transferQIdx;
        printf("Max Compute Shared Memory Size: %d KB", _devProps.limits.maxComputeSharedMemorySize / 1024);
        std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos = _defineExecQueues(pd.getQueueFamilyProperties());
        _dev = pd.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfos));  
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = pd.getQueueFamilyProperties();
        _buildCmds(pd.getQueueFamilyProperties());
        _pipelineCache = _dev.createPipelineCache(vk::PipelineCacheCreateInfo());
        _buildAllocators();
    }

    vk::Buffer createDeviceBuffer(size_t size, vk::BufferUsageFlags usage)
    {
        vk::BufferCreateInfo bufferInfo{
            vk::BufferCreateFlags(), size, usage, vk::SharingMode::eExclusive, 0, nullptr, nullptr};
        vk::Buffer buffer = _dev.createBuffer(bufferInfo);
        vk::MemoryRequirements memReq = _dev.getBufferMemoryRequirements(buffer);
        
        chunk_t chunk{{}, buffer, {}, 0, 0, nullptr};
        for(auto& allocator : _allocators)
        {
            if((static_cast<vk::MemoryPropertyFlags>(memReq.memoryTypeBits) & allocator.first) )
            {
                chunk = allocator.second.allocate_buffer(_dev, chunk, size);
                chunk.memoryPropertyFlags = allocator.first;
                _bufferChunks.emplace(buffer, chunk);
                break;
            }
        }
            
        
        // if ((usage & vk::BufferUsageFlagBits::eTransferSrc) == vk::BufferUsageFlagBits::eTransferDst ||
        //     (usage & vk::BufferUsageFlagBits::eTransferDst) == vk::BufferUsageFlagBits::eTransferDst)
        //      chunk.data = _dev.mapMemory(_allocators.at(chunk.memoryPropertyFlags).get_memory(chunk.buddyIdx), chunk.meta.offset, size);

    	
        return buffer;

    }

    vk::Image createImage(size_t size, vk::Format format, vk::ImageUsageFlags usage)
    {
        return vk::Image{};
    }

    /* dev(const dev&) = delete;
     dev& operator=(const dev&) = delete;*/
    dev(dev &&odev)
        : _dev(std::move(odev._dev)), _allocators(std::move(odev._allocators)),
          _compute_cmds(std::move(odev._compute_cmds)), _graphics_cmds(std::move(odev._graphics_cmds)),
          _transfer_cmds(std::move(odev._transfer_cmds)), _memProps(std::move(odev._memProps)),
          _devProps(std::move(odev._devProps)), _devProps2(std::move(odev._devProps2)),
          _devProps3(std::move(odev._devProps3)), _devProps4(std::move(odev._devProps4))
    {
    }

    dev &operator=(dev &&odev)
    {
        if (this != &odev)
        {
            _dev = std::move(odev._dev);
            _allocators = std::move(odev._allocators);
            _compute_cmds = std::move(odev._compute_cmds);
            _graphics_cmds = std::move(odev._graphics_cmds);
            _transfer_cmds = std::move(odev._transfer_cmds);
            _memProps = std::move(odev._memProps);
            _devProps = std::move(odev._devProps);
            _devProps2 = std::move(odev._devProps2);
            _devProps3 = std::move(odev._devProps3);
            _devProps4 = std::move(odev._devProps4);
        }
        return *this;
    }

    void destroy()
    {
        for (auto &allocs : _allocators)
            allocs.second.destroy(_dev);
        if (_dev != nullptr)
            _dev.destroy();
    }

    compPgrm define_shader_program(uint8_t* shader, size_t shadersize, const std::vector<vk::Buffer> &buffers)
    {
        auto pgrm = compPgrm();
        pgrm.defineShader(_dev, shader, shadersize);
        pgrm.definePipelineLayout(_dev, _pipelineCache);
        for (auto &buf : buffers)
        {
            auto chunk = _bufferChunks.at(buf);   
            pgrm.addBuffer(&chunk);
        }
        pgrm.definePipelineLayout(_dev, _pipelineCache);
        return pgrm;
    }

    void execute(const compPgrm& pgrm)
    {
        _compute_cmds.at(0).execute(_dev, pgrm);
    }

    ~dev() {};
};

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

inline void dev::_buildAllocators()
{
    size_t heapIdx;
    std::map<size_t, std::vector<vk::MemoryType>> memTypes;
    for (uint32_t i = 0; i < _memProps.memoryTypeCount; ++i)
    {
        auto type = _memProps.memoryTypes[i];
        heapIdx = type.heapIndex;
        if (memTypes.find(heapIdx) == memTypes.end())
        {
            memTypes.emplace(heapIdx, std::vector<vk::MemoryType>());
            memTypes.at(heapIdx).emplace_back(type);
        }
        else
        {
            memTypes.at(heapIdx).emplace_back(type);
        }
    }

    for (auto &h : memTypes)
    {
        std::vector<std::pair<size_t, size_t>> memTypeSize;
        std::map<size_t, size_t> memTypeIndex;
        std::map<size_t, double> memSizes;
        size_t total = 0;
        size_t idx = 0;

        for (auto type : h.second)
        {
            memTypeSize.emplace_back(idx, 0);
            if ((type.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) ==
                vk::MemoryPropertyFlagBits::eDeviceLocal)
            {
                memTypeIndex[idx] |= 1;
                memTypeSize[idx].second++;
                ++total;
            }
            if ((type.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) ==
                vk::MemoryPropertyFlagBits::eHostVisible)
            {
                memTypeIndex[idx] |= 2;
                memTypeSize[idx].second++;
                ++total;
            }
            if ((type.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent) ==
                vk::MemoryPropertyFlagBits::eHostCoherent)
            {
                memTypeIndex[idx] |= 4;
                memTypeSize[idx].second++;
                ++total;
            }
            if ((type.propertyFlags & vk::MemoryPropertyFlagBits::eHostCached) ==
                vk::MemoryPropertyFlagBits::eHostCached)
            {
                memTypeIndex[idx] |= 8;
                memTypeSize[idx].second++;
                ++total;
            }
            if ((type.propertyFlags & vk::MemoryPropertyFlagBits::eLazilyAllocated) ==
                vk::MemoryPropertyFlagBits::eLazilyAllocated)
            {
                memTypeIndex[idx] |= 16;
                memTypeSize[idx].second++;
                ++total;
            }
            if ((type.propertyFlags & vk::MemoryPropertyFlagBits::eProtected) == vk::MemoryPropertyFlagBits::eProtected)
            {
                memTypeIndex[idx] |= 32;
                memTypeSize[idx].second++;
                ++total;
            }
            ++idx;
        }

        std::vector<std::pair<vk::MemoryType, size_t>> memTypesSorted;
        std::sort(memTypeSize.begin(), memTypeSize.end(),
                  [](std::pair<size_t, size_t> &a, std::pair<size_t, size_t> &b) { return a.second > b.second; });
        size_t foundBitMasks = 0;
        for (auto &type : memTypeSize)
        {
            bool broke = false;
            auto mask = memTypeIndex[type.first];
            if (foundBitMasks & mask)
                continue;
            foundBitMasks |= mask;
            memTypesSorted.emplace_back(h.second[type.first], type.first);
        }

        auto heap = _memProps.memoryHeaps[h.first];
        for(auto &type : memTypesSorted)
        {
            _allocators.emplace(
                type.first.propertyFlags,
                allocator(_dev, heap.size, _devProps2.properties.limits.minStorageBufferOffsetAlignment, type.first.heapIndex));
        }
    }
}

inline void dev::_buildCmds(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties)
{   
    size_t j = 0;
    for(auto i = 0; i < queueFamilyProperties.size(); ++i)
    {
        auto cf = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute;
        auto gf = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics;
        auto tf = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer;
        
        for(auto k = 0; k < queueFamilyProperties[i].queueCount; ++k)
        {
            q cq(_dev, i, j+k);
            if(cf)
            {
                _compute_cmds.emplace_back(cq);
            }
            if(gf)
            {
                _graphics_cmds.emplace_back(cq);
            }
            if(tf)
            {
                _transfer_cmds.emplace_back(cq);
            }
        }
        j += queueFamilyProperties[i].queueCount;
    }
}



