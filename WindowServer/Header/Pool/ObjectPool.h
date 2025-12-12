#pragma once
#include "MemoryPool.h"

template<typename TYPE>
class ObjectPool 
{
private:
	ObjectPool(MemoryPool* InMemoryPool) :
		TypeSize(sizeof(TYPE)), MemoryPool(InMemoryPool)
	{
		InitializeSListHead(&UnAllocateObjectList);
		AllocationMutex = CreateMutex(nullptr, FALSE, nullptr);
		if (AllocationMutex == nullptr)
		{
			std::cout << "AllocationMutex is nullptr" << std::endl;
			exit(-1);
		}

		AllocateMemory();
	}
public:

	~ObjectPool()
	{
		CloseHandle(AllocationMutex);
		InterlockedFlushSList(&UnAllocateObjectList);
		for (int i = 0; i < HeadMemory.size(); i ++)
		{
			MemoryPool->Free(&HeadMemory[i]);
		}
	}

public:
	static std::shared_ptr<ObjectPool> CreateObjectPool(MemoryPool* InMemoryPool);

public:
	std::vector<TYPE*>	AllocateObjects(size_t size);
	TYPE* AllocateObject();
	void  FreeObject(TYPE** InObject);

private:
	void AllocateMemory();

private:
	const size_t TypeSize;

private:
	std::vector<void*> HeadMemory;
	MemoryPool* MemoryPool;


	__declspec(align(16))
	SLIST_HEADER UnAllocateObjectList;
	HANDLE AllocationMutex;
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
	PSLIST_ENTRY element = nullptr; 
	while (true)
	{
		try
		{
			element = InterlockedPopEntrySList(&UnAllocateObjectList);
			if (element != nullptr) break;

			MUTEX_CHECK(WaitForSingleObject(AllocationMutex, INFINITE));
			USHORT depth = QueryDepthSList(&UnAllocateObjectList);
			if (depth == 0)
			{
				AllocateMemory();
			}
			ReleaseMutex(AllocationMutex);
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
		
	}
	
	TYPE* resultObject = new(element) TYPE;
	return resultObject;
}

template <typename TYPE>
std::vector<TYPE*> ObjectPool<TYPE>::AllocateObjects(size_t size)
{
	std::vector<TYPE*> v(size, nullptr);
	for (int i = 0; i < size; i ++)
	{
		v[i] = AllocateObject();
	}
	return std::move(v);
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
	if (memory == nullptr)
	{
		std::cout << "memory is empty" << std::endl;
		return;
	}
	HeadMemory.push_back(memory);
	size_t index = 0;

	while (index < MemoryPool->GetGroupSize())
	{
		InterlockedPushEntrySList(&UnAllocateObjectList, reinterpret_cast<PSLIST_ENTRY>(memory + index));
		index += TypeSize;
	}
}
