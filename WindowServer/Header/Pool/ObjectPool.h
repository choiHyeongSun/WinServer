#pragma once
#include "MemoryPool.h"

template<typename TYPE>
class ObjectPool
{

public:

	static std::shared_ptr<ObjectPool> CreateObjectPool(MemoryPool* InMemoryPool);
	~ObjectPool()
	{
		InterlockedFlushSList(&UnAllocateObjectList);
		for (int i = 0; i < HeadMemory.size(); i ++)
		{
			MemoryPool->Free(&HeadMemory[i]);
		}
	}


private:
	ObjectPool(MemoryPool* InMemoryPool) :
		TypeSize(sizeof(TYPE)), MemoryPool(InMemoryPool)
	{
		AllocateMemory();
		InitializeSListHead(&UnAllocateObjectList);
	}

private:
	MemoryPool* MemoryPool;
	std::vector<void*> HeadMemory;
	size_t TypeSize;

	SLIST_HEADER UnAllocateObjectList;

public:
	TYPE* AllocateObject();
	void FreeObject(TYPE** InObject);

private:
	void AllocateMemory();


};

template <typename TYPE>
std::shared_ptr<ObjectPool<TYPE>> ObjectPool<TYPE>::CreateObjectPool(::MemoryPool* InMemoryPool)
{
	std::shared_ptr<ObjectPool> objectPool = std::shared_ptr<ObjectPool>(new ObjectPool(InMemoryPool));
	if (objectPool == nullptr)
	{
		std::cout << "Thread Pool failed to create";
		return nullptr;
	}
	return objectPool;
}

template <typename TYPE>
TYPE* ObjectPool<TYPE>::AllocateObject()
{
	USHORT depth = QueryDepthSList(&UnAllocateObjectList);
	if (depth == 0)
	{
		AllocateMemory();
	}
	PSLIST_ENTRY element = InterlockedPopEntrySList(&UnAllocateObjectList);
	TYPE* resultObject = new(element) TYPE;
	return resultObject;
}

template <typename TYPE>
void ObjectPool<TYPE>::FreeObject(TYPE** InObject)
{
	(*InObject)->~TYPE();
	memset(*InObject, 0, TypeSize);
	InterlockedPushEntrySList(&UnAllocateObjectList, reinterpret_cast<PSLIST_ENTRY>(*InObject));
	*InObject = nullptr;
}

template <typename TYPE>
void ObjectPool<TYPE>::AllocateMemory()
{
	char* memory = static_cast<char*>(MemoryPool->Allocate());
	HeadMemory.push_back(memory);
	size_t index = 0;

	while (index < MemoryPool->GetGroupSize())
	{
		InterlockedPushEntrySList(&UnAllocateObjectList, reinterpret_cast<PSLIST_ENTRY>(memory + index));
		index += TypeSize;
	}
}
