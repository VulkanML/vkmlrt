#if defined(_MSC_VER)
// no need to ignore any warnings with MSVC
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#else
// unknow compiler... just ignore the warnings for yourselves ;)
#endif
#include "rt.h"

namespace vkmlrt {
    void init(std::vector<vkmlrt_device>& devices)
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
        appInfo.apiVersion = VK_API_VERSION_1_2;

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
            VolkDeviceTable t = {};
            volkLoadDeviceTable(&t, ld);

            VmaVulkanFunctions vmaFuncs = {};
            vmaFuncs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
            vmaFuncs.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
            vmaFuncs.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
            vmaFuncs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
            vmaFuncs.vkAllocateMemory = t.vkAllocateMemory;
            vmaFuncs.vkFreeMemory = t.vkFreeMemory;
            vmaFuncs.vkMapMemory = t.vkMapMemory;
            vmaFuncs.vkUnmapMemory = t.vkUnmapMemory;
            vmaFuncs.vkFlushMappedMemoryRanges = t.vkFlushMappedMemoryRanges;
            vmaFuncs.vkInvalidateMappedMemoryRanges = t.vkInvalidateMappedMemoryRanges;
            vmaFuncs.vkBindBufferMemory = t.vkBindBufferMemory;
            vmaFuncs.vkBindImageMemory = t.vkBindImageMemory;
            vmaFuncs.vkGetBufferMemoryRequirements = t.vkGetBufferMemoryRequirements;
            vmaFuncs.vkGetImageMemoryRequirements = t.vkGetImageMemoryRequirements;
            vmaFuncs.vkCreateBuffer = t.vkCreateBuffer;
            vmaFuncs.vkDestroyBuffer = t.vkDestroyBuffer;
            vmaFuncs.vkCreateImage = t.vkCreateImage;
            vmaFuncs.vkDestroyImage = t.vkDestroyImage;
            vmaFuncs.vkCmdCopyBuffer = t.vkCmdCopyBuffer;

            vmaFuncs.vkGetBufferMemoryRequirements2KHR = t.vkGetBufferMemoryRequirements2KHR;
            vmaFuncs.vkGetImageMemoryRequirements2KHR = t.vkGetImageMemoryRequirements2KHR;

            vmaFuncs.vkBindBufferMemory2KHR = t.vkBindBufferMemory2KHR;
            vmaFuncs.vkBindImageMemory2KHR = t.vkBindImageMemory2KHR;
            vmaFuncs.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;

            //vmaFuncs.vkGetDeviceBufferMemoryRequirements = devices[i].t.vkGetDeviceBufferMemoryRequirements;
            //vmaFuncs.vkGetDeviceImageMemoryRequirements = devices[i].t.vkGetDeviceImageMemoryRequirements;


            devices[i].pd = pd;
            devices[i].ld = ld;
            devices[i].a.a = nullptr;

            VmaAllocatorCreateInfo allocatorInfo = {};
            allocatorInfo.instance = instance;
            allocatorInfo.physicalDevice = pd;
            allocatorInfo.device = ld;
            allocatorInfo.pVulkanFunctions = &vmaFuncs;
            allocatorInfo.vulkanApiVersion = appInfo.apiVersion;
            vmaCreateAllocator(&allocatorInfo, &devices[i].a.a);
            devices[i].pools.resize(1);

            VkPipelineCacheCreateInfo pipelineCacheInfo = {};
            pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            vkCreatePipelineCache(ld, &pipelineCacheInfo, nullptr, &devices[i].pipelineCache);

			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = 0;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vkCreateCommandPool(ld, &poolInfo, nullptr, &devices[i].computePool);
            
            ++i;
        }

    } 

    VkDescriptorPool getPool(vkmlrt_device& device) {
        if (device.a.p.readyPools.size() > 0)
        {
            VkDescriptorPool pool = device.a.p.readyPools.back();
            device.a.p.readyPools.pop_back();
            return pool;
        }
        else
        {
            VkDescriptorPoolSize poolSizes[11] = {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 10 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 20 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 5 }
            };           

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = 0;
            poolInfo.maxSets = 1000;
            poolInfo.poolSizeCount = (uint32_t)11;
            poolInfo.pPoolSizes = poolSizes;

            VkDescriptorPool pool;
            vkCreateDescriptorPool(device.ld, &poolInfo, nullptr, &pool);
            return pool;
        }
    }

    void write_shader_pgrm(vkmlrt_device& device, vkmlrt_compute_pgrm& pgrm, const unsigned char* code, size_t size) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = (uint32_t*)code;
        vkCreateShaderModule(device.ld, &createInfo, nullptr, &pgrm.shaderModule);
    }

    void write_image(vkmlrt_compute_pgrm& pgrm, int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkShaderStageFlagBits stageFlag)
    {
        VkDescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        layoutBinding.stageFlags = stageFlag;

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = layout;
        imageInfo.imageView = image;
        imageInfo.sampler = sampler;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;
        pgrm.d.writes.push_back(write);
    }

    void write_buffer(vkmlrt_device& device, vkmlrt_compute_pgrm& pgrm, int binding, VkBuffer buffer, VkShaderStageFlagBits stageFlag)
    {
		auto alloc = device.buffers.at(buffer);
        
        VkDescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBinding.stageFlags = stageFlag;

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = alloc->GetOffset();
        bufferInfo.range = alloc->GetSize();

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;
        pgrm.d.writes.push_back(write);
    }

	
    void write_compute_program(vkmlrt_device& device, vkmlrt_compute_pgrm& pgrm)
    {
        auto pool = getPool(device);
        if (device.a.p.currentPool == VK_NULL_HANDLE)
        {
            device.a.p.currentPool = getPool(device);
            device.a.p.usedPools.push_back(device.a.p.currentPool);
        }

        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = device.a.p.currentPool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &pgrm.layout;

        VkDescriptorSet set;
        auto res = vkAllocateDescriptorSets(device.ld, &alloc_info, &set);
        
        if (VK_ERROR_FRAGMENTATION == res || VK_ERROR_OUT_OF_POOL_MEMORY == res)
        {
            device.a.p.currentPool = getPool(device);
            device.a.p.usedPools.push_back(device.a.p.currentPool);
            res = vkAllocateDescriptorSets(device.ld, &alloc_info, &set);            
        }
       
		vkUpdateDescriptorSets(device.ld, (uint32_t)pgrm.d.writes.size(), pgrm.d.writes.data(), 0, nullptr);

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(pgrm.bindings.size());
		layoutInfo.pBindings = pgrm.bindings.data();      
        vkCreateDescriptorSetLayout(device.ld, &layoutInfo, nullptr, &pgrm.layout);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &pgrm.layout;
        VkPipelineLayout pipelineLayout;
		vkCreatePipelineLayout(device.ld, &pipelineLayoutInfo, nullptr, &pipelineLayout);

		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageInfo.module = pgrm.shaderModule;
		shaderStageInfo.pName = "main";
		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = shaderStageInfo;
		pipelineInfo.layout = pipelineLayout;
		vkCreateComputePipelines(device.ld, device.pipelineCache, 1, &pipelineInfo, nullptr, &pgrm.pipeline);

    }


}

