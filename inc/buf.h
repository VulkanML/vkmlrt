#include <vulkan/vulkan.h>
#include <vector>
#include <deque>
#include <unordered_map>
#include <algorithm>
namespace vkmlrt 
{

	struct DescriptorAllocator 
	{
		struct PoolSizeRatio 
		{
			VkDescriptorType type;
			float ratio;
		};

		void init(VkDevice device, uint32_t initialSets, std::vector<PoolSizeRatio> poolRatios) 
		{
			ratios.clear();
			ratios = poolRatios;
			setsPerPool = initialSets * 1.5f;
			readyPools.push_back(create_pool(device, initialSets, ratios));

		}
		void clear_pools(VkDevice device)
		{
			for (auto p : readyPools) {
				vkResetDescriptorPool(device, p, 0);
			}
			for (auto p : fullPools) {
				vkResetDescriptorPool(device, p, 0);
				readyPools.push_back(p);
			}
			fullPools.clear();
		}
		void destroy_pools(VkDevice device)
		{
			for (auto p : readyPools) 
				vkDestroyDescriptorPool(device, p, nullptr);
			readyPools.clear();

			for (auto p : fullPools) 
				vkDestroyDescriptorPool(device, p, nullptr);
			
			fullPools.clear();
		}

		VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr) 
		{
			VkDescriptorPool pool = get_pool(device);
			VkDescriptorSetAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &layout;
			alloc_info.pNext = pNext;

			VkDescriptorSet descriptorSet;
			auto res = vkAllocateDescriptorSets(device, &alloc_info, &descriptorSet);
			if (res == VK_ERROR_OUT_OF_POOL_MEMORY || res == VK_ERROR_FRAGMENTED_POOL)
			{
				fullPools.push_back(pool);
				pool = get_pool(device);
				alloc_info.descriptorPool = pool;
				res = vkAllocateDescriptorSets(device, &alloc_info, &descriptorSet);
			}

			readyPools.push_back(pool);
			return descriptorSet;
		}

	private:
		VkDescriptorPool get_pool(VkDevice device)
		{
			VkDescriptorPool newPool;
			if (readyPools.size() != 0) 
			{
				newPool = readyPools.back();
				readyPools.pop_back();
			}
			else {
				//need to create a new pool
				newPool = create_pool(device, setsPerPool, ratios);
				setsPerPool = setsPerPool * 1.5;
				if (setsPerPool > 4092) 
					setsPerPool = 4092;
				
			}

			return newPool;
		}

		VkDescriptorPool create_pool(VkDevice device, uint32_t setCount, std::vector<PoolSizeRatio> poolRatios) 
		{
			std::vector<VkDescriptorPoolSize> poolSizes;
			for (PoolSizeRatio ratio : poolRatios) 
			{
				VkDescriptorPoolSize poolSize = { };
				poolSize.type = ratio.type;
				poolSize.descriptorCount = ratio.ratio * setCount;
				poolSizes.push_back(poolSize);
			}

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = 0;
			pool_info.maxSets = setCount;
			pool_info.poolSizeCount = (uint32_t)poolSizes.size();
			pool_info.pPoolSizes = poolSizes.data();

			VkDescriptorPool newPool;
			vkCreateDescriptorPool(device, &pool_info, nullptr, &newPool);
			return newPool;
		}

		std::vector<PoolSizeRatio> ratios;
		std::vector<VkDescriptorPool> fullPools;
		std::vector<VkDescriptorPool> readyPools;
		uint32_t setsPerPool;

	};

	struct DescriptorWriter 
	{
		std::deque<VkDescriptorImageInfo> imageInfos;
		std::deque<VkDescriptorBufferInfo> bufferInfos;
		std::vector<VkWriteDescriptorSet> writes;

		void write_image(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type, ) 
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = layout;
			imageInfo.imageView = image;
			imageInfo.sampler = sampler;
			imageInfos.push_back(imageInfo);

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = binding;
			write.dstSet = VK_NULL_HANDLE;
			write.dstArrayElement = 0;
			write.descriptorType = type;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfos.back();
			writes.push_back(write);
		}
		void write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type) 
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = buffer;
			bufferInfo.offset = offset;
			bufferInfo.range = size;
			bufferInfos.push_back(bufferInfo);

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = binding;
			write.dstArrayElement = 0;
			write.descriptorType = type;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfos.back();
			writes.push_back(write);
		}

		void clear() 
		{
			imageInfos.clear();
			bufferInfos.clear();
			writes.clear();
		}
		void update_set(VkDevice device, VkDescriptorSet set)
		{
			for (auto& write : writes) 
				write.dstSet = set;			
			vkUpdateDescriptorSets(device, (uint32_t)writes.size(), writes.data(), 0, nullptr);
		}


	};


	class DescriptorLayoutCache {
	public:
		void init(VkDevice newDevice)
		{
			device = newDevice;
		}
		void cleanup()
		{
			//delete every descriptor layout held
			for (auto pair : layoutCache) {
				vkDestroyDescriptorSetLayout(device, pair.second, nullptr);
			}
		}

		VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info) {
			DescriptorLayoutInfo layoutinfo;
			layoutinfo.bindings.reserve(info->bindingCount);
			bool isSorted = true;
			int lastBinding = -1;

			//copy from the direct info struct into our own one
			for (int i = 0; i < info->bindingCount; i++)
			{
				layoutinfo.bindings.push_back(info->pBindings[i]);

				//check that the bindings are in strict increasing order
				if (info->pBindings[i].binding > lastBinding)
					lastBinding = info->pBindings[i].binding;
				else
					isSorted = false;

			}
			//sort the bindings if they aren't in order
			if (!isSorted)
				std::sort(layoutinfo.bindings.begin(), layoutinfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
				return a.binding < b.binding;
					});

			//try to grab from cache
			auto it = layoutCache.find(layoutinfo);
			if (it != layoutCache.end())
			{
				return (*it).second;
			}
			else
			{
				//create a new one (not found)
				VkDescriptorSetLayout layout;
				vkCreateDescriptorSetLayout(device, info, nullptr, &layout);

				//add to cache
				layoutCache[layoutinfo] = layout;
				return layout;
			}
		}

		struct DescriptorLayoutInfo {
			//good idea to turn this into a inlined array
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			bool operator==(const DescriptorLayoutInfo& other) const
			{
				if (other.bindings.size() != bindings.size())
					return false;
				else
				{
					//compare each of the bindings is the same. Bindings are sorted so they will match
					for (int i = 0; i < bindings.size(); i++) {
						if (other.bindings[i].binding != bindings[i].binding) {
							return false;
						}
						if (other.bindings[i].descriptorType != bindings[i].descriptorType) {
							return false;
						}
						if (other.bindings[i].descriptorCount != bindings[i].descriptorCount) {
							return false;
						}
						if (other.bindings[i].stageFlags != bindings[i].stageFlags) {
							return false;
						}
					}
					return true;
				}
			}

			size_t hash() const {
				using std::size_t;
				using std::hash;

				size_t result = hash<size_t>()(bindings.size());

				for (const VkDescriptorSetLayoutBinding& b : bindings)
				{
					//pack the binding data into a single int64. Not fully correct but it's ok
					size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

					//shuffle the packed binding data and xor it with the main hash
					result ^= hash<size_t>()(binding_hash);
				}

				return result;
			}
		};



	private:

		struct DescriptorLayoutHash {

			std::size_t operator()(const DescriptorLayoutInfo& k) const {
				return k.hash();
			}
		};

		std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;
		VkDevice device;
	};

	struct DescriptorLayoutBuilder 
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		void add_binding(uint32_t binding, VkDescriptorType type) 
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding;
			layoutBinding.descriptorType = type;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			layoutBinding.pImmutableSamplers = nullptr;
			bindings.push_back(layoutBinding);

		}
		void clear() 
		{
			bindings.clear();
		}

		VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, VkDescriptorSetLayoutCreateFlags flags = 0) 
		{
			for (auto& b : bindings) 
				b.stageFlags |= shaderStages;
			

			VkDescriptorSetLayoutCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.pBindings = bindings.data();
			info.bindingCount = (uint32_t)bindings.size();
			info.flags = flags;

			VkDescriptorSetLayout set;
			auto res = vkCreateDescriptorSetLayout(device, &info, nullptr, &set);
			if (res != VK_SUCCESS)
				printf("Failed to create descriptor set layout\n");
			
			return set;
		}

	};




} // namespace vkmlrt

