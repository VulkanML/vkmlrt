#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_hash.hpp>
#include <unordered_map>



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
    size_t memoryKey;
    void *data;
};


class Buddy
{
    size_t _total_size;
    size_t _min_block_size;
    uint32_t _type_idx;
    vk::DeviceMemory _memory;
    std::vector<std::vector<meta_data>> _free_blocks;
    void _splitBlock(meta_data &block, size_t size)
    {
        while (block.size > size)
        {
            block.size /= 2;
            meta_data buddy = {block.size, block.offset ^ block.size, true};
            _free_blocks[static_cast<size_t>(log2(block.size))].push_back(buddy);
        }
    }
    void _mergeBlock(meta_data &block)
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
        _free_blocks.back().push_back({totalSize, 0, true});
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
        size = static_cast<size_t>(tmp * tmp);
        if (size < _min_block_size)
            size = _min_block_size;

        for (size_t i = static_cast<size_t>(log2(size)); i < _free_blocks.size(); ++i)
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
        return meta_data{0, 0, false};
    }

    size_t getTypeId()
    {
        return _type_idx;
    }
    vk::DeviceMemory getMemory()
    {
        return _memory;
    }
    void free(meta_data &block)
    {
        block.free = true;
        _mergeBlock(block);
    }
    void destroy(vk::Device dev)
    {
        dev.free(_memory);
    }
};



class allocator
{
    std::unordered_map<vk::Buffer, chunk_t> _chunks_maps;
    std::vector<Buddy> _buddies;
    size_t _alignment;
    size_t _max_total_size;
    vk::MemoryType _memType;

  public:
    allocator() = delete;

    allocator(vk::Device dev, size_t TotalSize, size_t alignment,
              vk::MemoryType type)
        : _alignment(alignment), _max_total_size(TotalSize), _memType(type)
    {
        auto range = TotalSize / 2147483648llu; 
        for (auto j = 0; j < range; ++j)
            _buddies.emplace_back(dev, type.heapIndex, 2147483648llu, alignment);
    };

    void destroy(vk::Device dev)
    {
        for (auto &buddy : _buddies)
            buddy.destroy(dev);
    }

    chunk_t& allocate_buffer(vk::Buffer buffer, vk::MemoryRequirements memReq, size_t size)
    {
        chunk_t chunk{{}, buffer, 0, nullptr};
        size_t idx = 0;
        for (auto buddy : _buddies)
        {
            if (memReq.memoryTypeBits & buddy.getTypeId())
            {
                chunk.meta = buddy.allocate(size);
                chunk.memoryKey = idx;
            }
            ++idx;
        }
        return _chunks_maps.at(buffer) = chunk;
    }
};