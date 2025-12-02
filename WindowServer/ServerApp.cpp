#include "MainPCH.h"
#include "ServerApp.h"

#include "Pool/MemoryPool.h"
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
	ThreadPooling = ThreadPool::CreateThreadPool(ThreadPoolMaxCount, ThreadPoolMaxJobCount);
	MemoryPooling = MemoryPool::CreateMemoryPool(MemoryPoolSize, MemoryPoolGroup);
	ObjectPooling = ObjectPool<test_struct>::CreateObjectPool(MemoryPooling.get());
}

ServerApp::~ServerApp()
{

}

INT32 ServerApp::Run()
{
	const int testCase = 100000000;
	auto start = std::chrono::high_resolution_clock::now();
	{
		std::vector<test_struct*> bucket;
		for (int i = 0; i < testCase; i++)
		{
			test_struct* test = ObjectPooling->AllocateObject();
			test->a[0] = i;
			bucket.push_back(test);
		}

		for (int i = 0; i < testCase; i++)
		{
			ObjectPooling->FreeObject(&bucket[i]);
		}
	}
	
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << "오브젝트 풀링 실행 시간 : " << duration.count() << std::endl;
	{
		start = std::chrono::high_resolution_clock::now();
		std::vector<test_struct*> bucket;

		for (int i = 0; i < testCase; i++)
		{
			test_struct* test = new test_struct();
			test->a[0] = i;
			bucket.push_back(test);
		}
		for (int i = 0; i < testCase; i++)
		{
			delete bucket[i];
		}
	}


	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << "new And Delete 실행 시간 : " << duration.count() << std::endl;

	ThreadPooling->WaitForAllThreas();

	return 0;
}

