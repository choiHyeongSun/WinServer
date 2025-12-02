#pragma once

class MemoryPool
{
public:
	static std::shared_ptr<MemoryPool> CreateMemoryPool(size_t InSize, size_t InGroupSize);

private:
	MemoryPool() = delete;
	MemoryPool(size_t InSize, size_t InGroupSize) : Size(InSize), GroupSize(InGroupSize)
	{
		Memories.push_back(CreateMemory());
	}

public:
	~MemoryPool()
	{
		for (size_t i = 0; i < Memories.size(); i++)
		{
			_aligned_free(Memories[i]);
		}
		Memories.clear();
	}

public:
	FORCEINLINE size_t GetGroupSize() const { return GroupSize; }


private:
	void* CreateMemory();

public:
	void* Allocate();
	void Free(void** InMemory);


private:
	size_t GroupSize = 0;
	size_t Size = 0;

private:
	std::vector<void*> Memories;
	std::queue<void*> UnAllocateMemory;
};
