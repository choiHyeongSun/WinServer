#include "MainPCH.h"
#include "ServerApp.h"
#include "Header/Pool/PoolingManager.h"
#include "Pool/ThreadPool.h"

const size_t ServerApp::ThreadPoolMaxCount = 10;
const size_t ServerApp::ThreadPoolMaxJobCount = 300;

const size_t ServerApp::MemoryPoolSize = 1024ull * 1024ull;
const size_t ServerApp::MemoryPoolGroup = 1024ull;

struct test_struct
{
	int a[4] = { 0 };
};

ServerApp::ServerApp()
{

}

ServerApp::~ServerApp()
{
}

INT32 ServerApp::Run()
{
	PoolingManager<test_struct>::CreateObjectPoolManager(MemoryPoolSize, MemoryPoolGroup);
	sockWrapper = BlockSocketWrapper::CreateSocketWrapper(PROTOCOL_TYPE::TCP);
	return 0;
}

