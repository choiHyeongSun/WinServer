#pragma once
#include <shared_mutex>

#include "Pool/MemoryPool.h"
#include "Pool/ObjectPool.h"

template<class TYPE>
class PoolingManager
{
public:
	~PoolingManager()
	{
		if (ObjectManager != nullptr)
			ObjectManager.reset();
	}

public:
	static void FreeObject(TYPE** Obj);
	static void FreeObject(void** Obj);
	static TYPE* GenerateObject();

private:
	static void CreateObjectPoolManager(size_t InSize, size_t InGroupSize);


private:
	static std::unique_ptr<PoolingManager<TYPE>> ObjectManager;
	static std::shared_mutex mutex;
	static std::once_flag initFlag;

	static const size_t MemoryPoolSize;
	static const size_t MemoryPoolGroup;

private:
	std::shared_ptr<MemoryPool> MemoryPooling = nullptr;
	std::shared_ptr<ObjectPool<TYPE>> ObjectPooling = nullptr;
};

template<class TYPE>
const size_t PoolingManager<TYPE>::MemoryPoolSize = 1024ull * 1024ull;
template<class TYPE>
const size_t PoolingManager<TYPE>::MemoryPoolGroup = 1024ull;


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
void PoolingManager<TYPE>::FreeObject(void** Obj)
{
	TYPE* casting = static_cast<TYPE*>((*Obj));
	ObjectManager->ObjectPooling->FreeObject(&casting);
	(*Obj) = nullptr;
}

template <class TYPE>
TYPE* PoolingManager<TYPE>::GenerateObject()
{
	
	std::call_once(initFlag, [&]() {
		CreateObjectPoolManager(MemoryPoolSize, MemoryPoolGroup);
		});

	if (ObjectManager == nullptr)
	{
		std::cout << "Generated Object Error" << std::endl;
		exit(-1);
	}

	return ObjectManager->ObjectPooling->AllocateObject();
}

template<class TYPE>
std::unique_ptr<PoolingManager<TYPE>> PoolingManager<TYPE>::ObjectManager = nullptr;

template<class TYPE>
std::shared_mutex PoolingManager<TYPE>::mutex;

template <class TYPE>
std::once_flag PoolingManager<TYPE>::initFlag;