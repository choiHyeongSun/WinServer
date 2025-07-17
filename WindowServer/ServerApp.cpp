#include "MainPCH.h"
#include "ServerApp.h"

#include "Pool/MemoryPool.h"
#include "Pool/ThreadPool.h"

const size_t ServerApp::ThreadPoolMaxCount = 10;
const size_t ServerApp::ThreadPoolMaxJobCount = 300;

const size_t ServerApp::MemoryPoolSize = 1024ull * 1024ull;
const size_t ServerApp::MemoryPoolGroup = 1024ull;



ServerApp::ServerApp()
{
	ThreadPooling = ThreadPool::CreateThreadPool(ThreadPoolMaxCount, ThreadPoolMaxJobCount);
	MemoryPooling = MemoryPool::CreateMemoryPool(MemoryPoolSize, MemoryPoolGroup);
}

ServerApp::~ServerApp()
{

}

INT32 ServerApp::Run()
{
	return 0;
}

