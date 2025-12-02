#pragma once
#include "Pool/ObjectPool.h"


struct test_struct;
class MemoryPool;
class ThreadPool;

class ServerApp
{
private:
	static const size_t ThreadPoolMaxCount;
	static const size_t ThreadPoolMaxJobCount;
	static const size_t MemoryPoolSize;
	static const size_t MemoryPoolGroup;

public:
	ServerApp();
	~ServerApp();

public:
	INT32 Run();

private:
	std::shared_ptr<ThreadPool> ThreadPooling;
	std::shared_ptr<MemoryPool> MemoryPooling;
	std::shared_ptr<ObjectPool<test_struct>> ObjectPooling;
};

