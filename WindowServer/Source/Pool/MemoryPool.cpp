#include "MainPCH.h"
#include "Pool/MemoryPool.h"

std::shared_ptr<MemoryPool> MemoryPool::CreateMemoryPool(size_t InSize, size_t InGroupSize)
{
	std::shared_ptr<MemoryPool> memoryPool = std::shared_ptr<MemoryPool>(new MemoryPool(InSize, InGroupSize));
	if (memoryPool == nullptr)
	{
		std::cout << "Thread Pool failed to create";
		return nullptr;
	}
	return memoryPool;
}

void* MemoryPool::CreateMemory()
{
	void* memory = _aligned_malloc(Size, MEMORY_ALLOCATION_ALIGNMENT);
	memset(memory, 0, Size);

	size_t index = 0;
	while (index < Size)
	{
		UnAllocateMemory.push(static_cast<char*>(memory) + index);
		index += GroupSize;
	}
	return memory;
}

void* MemoryPool::Allocate()
{
	if (UnAllocateMemory.empty())
	{
		Memories.push_back(CreateMemory());
	}
	void* memory = UnAllocateMemory.front(); UnAllocateMemory.pop();
	return memory;
}

void MemoryPool::Free(void** InMemory)
{
	memset((*InMemory), 0, GroupSize);
	void* realMemory = *InMemory;
	(*InMemory) = nullptr;
	UnAllocateMemory.push(realMemory);
}
