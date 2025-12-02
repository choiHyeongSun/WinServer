#pragma once
#include "Server/BlockSocketWrapper.h"


struct test_struct;
class MemoryPool;
class ThreadPool;

class ServerApp
{


public:
	ServerApp();
	~ServerApp();

public:
	INT32 Run();

private:
	static const size_t ThreadPoolMaxCount;
	static const size_t ThreadPoolMaxJobCount;
	static const size_t MemoryPoolSize;
	static const size_t MemoryPoolGroup;

	std::shared_ptr<BlockSocketWrapper> sockWrapper;
};

