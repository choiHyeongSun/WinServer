#pragma once
#include "Pool/MemoryPool.h"
#include "Pool/ObjectPool.h"

template<class TYPE>
class PoolingManager
{

public:
	static void CreateObjectPoolManager(size_t InSize, size_t InGroupSize);
	static void FreeObject(TYPE** Obj);
	static TYPE* GenerateObject();

private:
	static std::unique_ptr<PoolingManager<TYPE>> ObjectManager;

private:
	std::shared_ptr<MemoryPool> MemoryPooling = nullptr;
	std::shared_ptr<ObjectPool<TYPE>> ObjectPooling = nullptr;
};


template <class TYPE>
void PoolingManager<TYPE>::CreateObjectPoolManager(size_t InSize, size_t InGroupSize)
{
	if (sizeof(TYPE) % 16 != 0)
	{
		std::cout << "This object is not 16-byte aligned.";
		exit(-1);
	}

	if (ObjectManager == nullptr)
	{
		ObjectManager = std::make_unique<PoolingManager<TYPE>>();
		ObjectManager->MemoryPooling = MemoryPool::CreateMemoryPool(InSize, InGroupSize);
		ObjectManager->ObjectPooling = ObjectPool<TYPE>::CreateObjectPool(ObjectManager->MemoryPooling.get());
	}
}

template <class TYPE>
void PoolingManager<TYPE>::FreeObject(TYPE** Obj)
{
	ObjectManager->ObjectPooling->FreeObject(Obj);
}

template <class TYPE>
TYPE* PoolingManager<TYPE>::GenerateObject()
{
	TYPE* obj = ObjectManager->ObjectPooling->AllocateObject();
	if (obj == nullptr)
	{
		std::cout << "Generated Object Error" << std::endl;
		exit(-1);
	}

	return obj;
}

template<class TYPE>
std::unique_ptr<PoolingManager<TYPE>> PoolingManager<TYPE>::ObjectManager = nullptr;
