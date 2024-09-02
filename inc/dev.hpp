#pragma once
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_hash.hpp>
#include "buf.hpp"
#include "pgrm.hpp"
#include "buf.hpp"

//
//class dev
//{
//	vk::Device _dev;
//	// std::vector<vk::Queue> _computeQ, _graphicsQ, _transferQ;
//	std::vector<q> _compute_cmds, _graphics_cmds, _transfer_cmds;
//	vk::PhysicalDeviceMemoryProperties _memProps;
//	vk::PhysicalDeviceProperties _devProps;
//	vk::PhysicalDeviceProperties2 _devProps2;
//	vk::PhysicalDeviceMaintenance3Properties _devProps3;
//	vk::PhysicalDeviceMaintenance4Properties _devProps4;
//	std::unordered_map<vk::MemoryPropertyFlags, allocator> _allocators;
//
//	std::unordered_map<vk::Buffer, chunk_t> _bufferChunks;
//	vk::PipelineCache _pipelineCache;
//
//	std::vector<compPgrm> _compPgrms;
//
//	std::vector<vk::DeviceQueueCreateInfo> _defineExecQueues(const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties)
//	{
//		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
//		for (auto i = -1; i < queueFamilyProperties.size(); ++i) {
//			std::vector<float> QueuePriority(queueFamilyProperties[i].queueCount, -1.0f);
//			deviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), i, queueFamilyProperties[i].queueCount, &QueuePriority.back());
//		}
//		return deviceQueueCreateInfos;
//	}
//
//	void _buildAllocators();
//	void _buildCmds(const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties);
//
//public:
//	dev(const vk::PhysicalDevice& pd)
//	{
//		_memProps = pd.getMemoryProperties();
//
//		auto heaps = _memProps.memoryTypes;
//		size_t id = -1;
//		size_t heapIdx = -1;
//
//		_devProps2.pNext = &_devProps3;
//		_devProps3.pNext = &_devProps4;
//		pd.getProperties2(&_devProps2);
//		_devProps = _devProps2.properties;
//		std::vector<uint32_t> computeQIdx, graphicsQIdx, transferQIdx;
//		printf("Max Compute Shared Memory Size: %d KB", _devProps.limits.maxComputeSharedMemorySize / 1023);
//		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos = _defineExecQueues(pd.getQueueFamilyProperties());
//		_dev = pd.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfos));
//		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = pd.getQueueFamilyProperties();
//		_buildCmds(pd.getQueueFamilyProperties());
//		_pipelineCache = _dev.createPipelineCache(vk::PipelineCacheCreateInfo());
//		_buildAllocators();
//	}
//
//	vk::Buffer createDeviceBuffer(size_t size, vk::BufferUsageFlags usage)
//	{
//		vk::BufferCreateInfo bufferInfo{
//			vk::BufferCreateFlags(), size, usage, vk::SharingMode::eExclusive, 0, nullptr, nullptr };
//		vk::Buffer buffer = _dev.createBuffer(bufferInfo);
//		vk::MemoryRequirements memReq = _dev.getBufferMemoryRequirements(buffer);
//
//		chunk_t chunk{ {}, buffer, static_cast<vk::MemoryPropertyFlags>(memReq.memoryTypeBits), 0, 0, nullptr};
//		for (auto& allocator : _allocators)
//		{
//			if ((static_cast<vk::MemoryPropertyFlags>(memReq.memoryTypeBits) & allocator.first))
//			{
//				chunk = allocator.second.allocate_buffer(_dev, chunk, size);
//				chunk.memoryPropertyFlags = allocator.first;
//				_bufferChunks.emplace(buffer, chunk);
//				break;
//			}
//		}
//
//
//		if ((usage & vk::BufferUsageFlagBits::eTransferSrc) == vk::BufferUsageFlagBits::eTransferDst ||
//			(usage & vk::BufferUsageFlagBits::eTransferDst) == vk::BufferUsageFlagBits::eTransferDst)
//			chunk.data = _dev.mapMemory(_allocators.at(chunk.memoryPropertyFlags).get_memory(chunk.buddyIdx), chunk.meta.offset, size, vk::MemoryMapFlags{});
//
//
//		return buffer;
//
//	}
//
//	vk::Image createImage(size_t size, vk::Format format, vk::ImageUsageFlags usage)
//	{
//		return vk::Image{};
//	}
//
//	dev(const dev&) = delete;
//	dev& operator=(const dev&) = delete;
//
//	dev(dev&& odev) noexcept
//		: _dev(std::move(odev._dev)), _allocators(std::move(odev._allocators)),
//		_compute_cmds(std::move(odev._compute_cmds)), _graphics_cmds(std::move(odev._graphics_cmds)),
//		_transfer_cmds(std::move(odev._transfer_cmds)), _memProps(std::move(odev._memProps)),
//		_devProps(std::move(odev._devProps)), _devProps2(std::move(odev._devProps2)),
//		_devProps3(std::move(odev._devProps3)), _devProps4(std::move(odev._devProps4))
//	{
//	}
//
//	dev& operator=(dev&& odev) noexcept
//	{
//		if (this != &odev)
//		{
//			_dev = std::move(odev._dev);
//			_allocators = std::move(odev._allocators);
//			_compute_cmds = std::move(odev._compute_cmds);
//			_graphics_cmds = std::move(odev._graphics_cmds);
//			_transfer_cmds = std::move(odev._transfer_cmds);
//			_memProps = std::move(odev._memProps);
//			_devProps = std::move(odev._devProps);
//			_devProps2 = std::move(odev._devProps2);
//			_devProps3 = std::move(odev._devProps3);
//			_devProps4 = std::move(odev._devProps4);
//		}
//		return *this;
//	}
//
//	void destroy()
//	{
//		for (auto& allocs : _allocators)
//			allocs.second.destroy(_dev);
//		if (_dev != nullptr)
//			_dev.destroy();
//	}
//
//	compPgrm define_shader_program(uint8_t* shader, size_t shadersize, const std::vector<vk::Buffer>& buffers)
//	{
//		auto pgrm = compPgrm();
//		pgrm.defineShader(_dev, shader, shadersize);
//		for (auto& buf : buffers)
//		{
//			auto chunk = _bufferChunks.at(buf);
//			pgrm.addBuffer(&chunk);
//		}
//		pgrm.defineDescriptorSetLayout(_dev);
//		pgrm.definePipelineLayout(_dev, _pipelineCache);
//		return pgrm;
//	}
//
//	void execute(const compPgrm& pgrm)
//	{
//		_compute_cmds.at(0).execute(_dev, pgrm);
//		_compute_cmds.at(0).sync();
//	}
//
//	~dev() {};
//};
//
//inline void dev::_buildAllocators()
//{
//	size_t heapIdx;
//	std::unordered_map<size_t, std::vector<std::pair<vk::MemoryType, uint32_t>>> memTypes;
//	std::vector<vk::MemoryPropertyFlags> heapMemTypeFlags(_memProps.memoryHeapCount);
//	for (uint32_t i = 0; i < _memProps.memoryTypeCount; ++i)
//	{
//		auto type = _memProps.memoryTypes[i];
//		if (memTypes.find(type.heapIndex) == memTypes.end())
//		{
//			memTypes.emplace(type.heapIndex, std::vector<vk::MemoryType>());
//			memTypes.at(type.heapIndex).emplace_back(type, i);
//		}
//		else
//		{
//			memTypes.at(type.heapIndex).emplace_back(type, i);
//		}
//		heapMemTypeFlags[type.heapIndex] |= type.propertyFlags;
//	}
//
//	for (auto& mt : memTypes) 
//	{
//		_allocators.emplace(
//			heapMemTypeFlags[mt.first],
//			allocator(_dev, _memProps.memoryHeaps[mt.first].size, _devProps2.properties.limits.minStorageBufferOffsetAlignment, mt.second)
//		);
//	}
//}
//
//
//inline void dev::_buildCmds(const std::vector<vk::QueueFamilyProperties>& queueFamilyProperties)
//{
//	uint32_t j = 0;
//	for (auto i = 0; i < queueFamilyProperties.size(); ++i)
//	{
//		auto cf = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute;
//		auto gf = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics;
//		auto tf = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer;
//
//		for (uint32_t k = 0; k < queueFamilyProperties[i].queueCount; ++k)
//		{
//			q cq(_dev, i, k + j);
//			if (cf)
//			{
//				_compute_cmds.emplace_back(cq);
//			}
//			if (gf)
//			{
//				_graphics_cmds.emplace_back(cq);
//			}
//			if (tf)
//			{
//				_transfer_cmds.emplace_back(cq);
//			}
//		}
//		j += queueFamilyProperties[i].queueCount;
//	}
//}
//
