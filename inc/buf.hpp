#pragma once
#include <algorithm>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_hash.hpp>
#include <vector>
#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.3

#define block_size  1 << 15


struct meta_data
{
	size_t size;
	size_t offset;
	bool free;
};

struct chunk_t
{
	meta_data meta;
	union {
		vk::Buffer buffer;
		vk::Image image;
	};
	vk::MemoryPropertyFlags memoryPropertyFlags;
	vk::MemoryTypeBit
	size_t buddyIdx;
	size_t chunkIdx;
	void* data;
};


class Buddy
{
	size_t _total_size;
	size_t _min_block_size;
	uint32_t _type_idx;
	vk::DeviceMemory _memory;
	std::vector<std::vector<meta_data>> _free_blocks;
	void _splitBlock(meta_data& block, size_t size)
	{
		while (block.size > size)
		{
			block.size /= 2;
			meta_data buddy = { block.size, block.offset ^ block.size, true };
			_free_blocks[static_cast<size_t>(log2(block.size))].push_back(buddy);
		}
	}
	void _mergeBlock(meta_data& block)
	{
		size_t buddy_offset = block.offset ^ block.size;
		for (auto it = _free_blocks[static_cast<size_t>(log2(block.size))].begin();
			it != _free_blocks[static_cast<size_t>(log2(block.size))].end(); ++it)
		{
			if (it->offset == buddy_offset && it->free)
			{
				it->free = false;
				block.size *= 2;
				block.offset = std::min(block.offset, it->offset);
				_free_blocks[static_cast<size_t>(log2(static_cast<double>(block.size) / 2))].erase(it);
				_mergeBlock(block);
				return;
			}
		}
		_free_blocks[static_cast<size_t>(log2(block.size))].push_back(block);
	}

public:
	Buddy(vk::Device dev, uint32_t type_idx, size_t totalSize, size_t minBlockSize)
		: _total_size(totalSize), _min_block_size(minBlockSize), _type_idx(type_idx)
	{
		size_t levels = static_cast<size_t>(log2(_total_size) + 1);
		_free_blocks.resize(levels);
		_free_blocks.back().push_back({ totalSize, 0, true });
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = totalSize;

		allocInfo.memoryTypeIndex = _type_idx;
		_memory = dev.allocateMemory(allocInfo);
		if (_memory == nullptr)
			throw std::runtime_error("Failed to allocate memory");
	}

	meta_data allocate(size_t size)
	{
		auto tmp = ceil(log2(size));
		auto sz = static_cast<size_t>(tmp * tmp);
		if (size < _min_block_size)
			size = _min_block_size;

		for (size_t i = static_cast<size_t>(log2(sz)); i < _free_blocks.size(); ++i)
		{
			if (!_free_blocks[i].empty())
			{
				meta_data block = _free_blocks[i].back();
				_free_blocks[i].pop_back();
				block.free = false;
				_splitBlock(block, size);
				return block;
			}
		}
		return meta_data{ 0, 0, false };
	}

	size_t getTypeId()
	{
		return _type_idx;
	}
	vk::DeviceMemory getMemory()
	{
		return _memory;
	}
	void free(meta_data& block)
	{
		block.free = true;
		_mergeBlock(block);
	}
	void destroy(vk::Device dev)
	{
		dev.free(_memory);
	}

	bool canAllocateSize(size_t size)
	{
		auto tmp = ceil(log2(size));
		for (auto i = static_cast<size_t>(log2(size)); i < _free_blocks.size(); ++i) {
			if (!_free_blocks[i].empty())
				return true;
		}
		return false;
	}
};



class allocator
{
	std::vector<chunk_t> _chunks;
	//std::vector<Buddy> _buddies;
	std::unordered_map< vk::MemoryPropertyFlags, std::vector<Buddy>> _buddy_map;
	size_t _alignment;
	size_t _max_total_size;

public:
	allocator() = delete;

	allocator(vk::Device dev, size_t TotalSize, size_t alignment, const std::vector<std::pair<vk::MemoryType, uint32_t>>& memTypeIndex)
		: _alignment(alignment), _max_total_size(TotalSize)
	{
		//auto range = TotalSize / block_size;
		//_buddies.emplace_back(dev, memTypeIndex, block_size, alignment);
		for (auto memory_type : memTypeIndex) {
			_buddy_map[memory_type.first.propertyFlags].emplace_back(dev, memory_type.second, block_size, alignment);
		}
	};

	void destroy(vk::Device dev)
	{
		for (auto& buddy : _buddy_map) 
		{
			for(auto &buddy : buddy.second)
				buddy.destroy(dev);
		}
	}

	chunk_t& allocate_buffer(vk::Device& dev, chunk_t& chunk, size_t size)
	{
		size_t idx;
		


		for (idx = 0; idx < _buddies.size(); ++idx) {
			if (_buddies.at(idx).canAllocateSize(size))
			{
				chunk.meta = _buddies.at(idx).allocate(size);
				chunk.buddyIdx = idx;
				dev.bindBufferMemory(chunk.buffer, _buddies.at(idx).getMemory(), chunk.meta.offset);
				break;
			}
		}
		chunk.chunkIdx = _chunks.size();
		_chunks.push_back(chunk);
		return chunk;
	}

	vk::DeviceMemory 

	chunk_t& get_chunk(size_t idx) {
		return _chunks.at(idx);
	}

};