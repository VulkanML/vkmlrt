#pragma once
#include <vulkan/vulkan.hpp>
#include "buf.hpp"


class q;

class compPgrm
{
	vk::ShaderModule _shaderModule;
	vk::PipelineLayout _pipeLayout;
	std::vector<vk::DescriptorBufferInfo> _chunks;
	std::vector<vk::DescriptorSetLayoutBinding> _descSetBindings;
	vk::DescriptorSetLayout _descSetLayout = nullptr;
	vk::DescriptorPool _descPool;
	std::vector<vk::DescriptorSet> _descSets;
	vk::Pipeline _pipe;

public:
	compPgrm()
	{
	}

	void destroy(vk::Device& dev)
	{
		dev.destroyShaderModule(_shaderModule);
		dev.destroyDescriptorSetLayout(_descSetLayout);
	}
	// add buffers # 1
	void addBuffer(chunk_t* arg)
	{
		_descSetBindings.emplace_back(_descSetBindings.size(), vk::DescriptorType::eStorageBuffer, 1,
			vk::ShaderStageFlagBits::eCompute);
		vk::DescriptorBufferInfo descBuffer(arg->buffer, arg->meta.offset, arg->meta.size);
		_chunks.push_back(descBuffer);
	}

	// define descriptorSetLayout # 1.1
	void defineDescriptorSetLayout(vk::Device& dev)
	{
		vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(),
			_descSetBindings);
		_descSetLayout = dev.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo);
		vk::DescriptorPoolSize DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, static_cast<uint32_t>(_descSetBindings.size()));
		vk::DescriptorPoolCreateInfo DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 1, DescriptorPoolSize);
		_descPool = dev.createDescriptorPool(DescriptorPoolCreateInfo);
		vk::DescriptorSetAllocateInfo DescriptorSetAllocInfo(_descPool, 1, &_descSetLayout);
		_descSets = dev.allocateDescriptorSets(DescriptorSetAllocInfo);
		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
		for (auto i = 0; i < _chunks.size(); ++i)
			writeDescriptorSets.emplace_back(_descSets.front(), i, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &_chunks[i]);
		dev.updateDescriptorSets(writeDescriptorSets, {});
	}

	// define shader # 1
	void defineShader(vk::Device& dev, uint8_t* shader, size_t size)
	{
		vk::ShaderModuleCreateInfo ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), size,
			reinterpret_cast<const uint32_t*>(shader));
		_shaderModule = dev.createShaderModule(ShaderModuleCreateInfo);
	}

	// define pipelineLayout # 2
	void definePipelineLayout(vk::Device& dev, const vk::PipelineCache& cache)
	{
		vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), _descSetLayout);
		_pipeLayout = dev.createPipelineLayout(PipelineLayoutCreateInfo);

		vk::ComputePipelineCreateInfo ComputePipelineCreateInfo(
			vk::PipelineCreateFlags(),
			vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eCompute,
				_shaderModule, "main"),
			_pipeLayout);
		_pipe = dev.createComputePipeline(cache, ComputePipelineCreateInfo).value;
	}

	friend class q;
};


class graphicsPgrm {

};


class q
{
	vk::Queue _cmdQueue;
	vk::CommandPool _cmdPool;
	vk::CommandBuffer _primaryCmdBuffer;
	vk::CommandBuffer _secondaryCmdBuffer;
	vk::Fence _fence;

public:
	q(vk::Device& dev, uint32_t qFamIdx, uint32_t queueIndex)
	{
		_fence = dev.createFence(vk::FenceCreateInfo());
		_cmdQueue = dev.getQueue(qFamIdx, queueIndex);
		_cmdPool = dev.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), qFamIdx));
		_primaryCmdBuffer =
			dev.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_cmdPool, vk::CommandBufferLevel::ePrimary, 1)).front();
		_secondaryCmdBuffer = dev.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_cmdPool, vk::CommandBufferLevel::eSecondary, 1)).front();
	}

	void execute(vk::Device& dev, const compPgrm& pgrm)
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

	void destroy(vk::Device& dev)
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