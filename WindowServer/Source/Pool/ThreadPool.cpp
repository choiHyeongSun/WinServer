#include "MainPCH.h"
#include "Pool/ThreadPool.h"

DWORD WINAPI ThreadProc(LPVOID LpParam)
{
	ThreadPool* threadPool = static_cast<ThreadPool*>(LpParam);
	if (threadPool == nullptr)
		return 1;

	while (!threadPool->IsFinish)
	{
		DWORD dwWaitResult = WaitForSingleObject(threadPool->WorkerThreadEvent, INFINITE);
		if (dwWaitResult == WAIT_FAILED)
		{
			DWORD errorCode = GetLastError();
			printf("Wait failed. Error code: %lu\n", errorCode);
			exit(-1);
		}
		if (threadPool->IsFinish) break;

		std::function<void()> callback = threadPool->GetNextJob();
		if (callback == nullptr)
			continue;

		callback();
	}
	return 0;
}


std::shared_ptr<ThreadPool> ThreadPool::CreateThreadPool(size_t InThreadMaxNum, size_t InJobMaxCapacity)
{
	std::shared_ptr<ThreadPool> threadPool = std::shared_ptr<ThreadPool>(new ThreadPool(InThreadMaxNum, InJobMaxCapacity));
	if (threadPool == nullptr)
	{
		std::cout << "Thread Pool failed to create";
		return nullptr;
	}
	return threadPool;
}

ThreadPool::ThreadPool(size_t InThreadMaxNum, size_t InJobMaxCapacity) :
	MaxThreadNum(InThreadMaxNum), JobMaxCapacity(InJobMaxCapacity), IsFinish(false)
{
	WorkerThreadEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (WorkerThreadEvent == nullptr)
	{
		std::cout << "Event Create Failed" << std::endl;
		exit(-1);
	}

	for (size_t i = 0; i < MaxThreadNum; i ++)
	{
		HANDLE threadHandle = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);
		if (threadHandle == nullptr)
		{
			std::cout << "Thread Create Failed" << std::endl;
			exit(-1);
		}
		Works.push_back(threadHandle);
	}

	Mutex = CreateMutex(nullptr, FALSE, nullptr);
	if (Mutex == nullptr)
	{
		std::cout << "Mutex Create Failed" << std::endl;
		exit(-1);
			
	}
}
ThreadPool::~ThreadPool()
{
	CloseHandle(Mutex);
	for (HANDLE workThread : Works)
	{
		CloseHandle(workThread);
	}
}



bool ThreadPool::SubmitJob(const std::function<void()>& InJob)
{
	if (Jobs.size() >= JobMaxCapacity)
	{
		return false;
	}

	DWORD dwWaitResult = WaitForSingleObject(Mutex, INFINITE);
	if (dwWaitResult == WAIT_FAILED)
	{
		DWORD errorCode = GetLastError();
		printf("Wait failed. Error code: %lu\n", errorCode);
		exit(-1);
	}
	Jobs.push(InJob);
	ReleaseMutex(Mutex);
	return true;
}

std::function<void()> ThreadPool::GetNextJob()
{

	DWORD dwWaitResult = WaitForSingleObject(Mutex, INFINITE);
	if (dwWaitResult == WAIT_FAILED)
	{
		DWORD errorCode = GetLastError();
		printf("Wait failed. Error code: %lu\n", errorCode);
		exit(-1);
	}
	if (Jobs.empty())
	{
		return nullptr;
	}
	std::function<void()> callback = Jobs.front(); Jobs.pop();
	ReleaseMutex(Mutex);
	return callback;
}

void ThreadPool::ExecutionThread()
{
	size_t size = Jobs.size();
	for (int i = 0; i < size; i++)
	{
		if (i > MaxThreadNum)
		{
			break;
		}
		SetEvent(WorkerThreadEvent);
	}
}

bool ThreadPool::IsHasJobs() const
{
	return !Jobs.empty();
}

void ThreadPool::WaitForAllThreas()
{
	for (int i = 0; i < MaxThreadNum; i ++)
	{
		DWORD dwWaitResult = WaitForSingleObject(Works[i], INFINITE);
		if (dwWaitResult == WAIT_FAILED)
		{
			DWORD errorCode = GetLastError();
			printf("Wait failed. Error code: %lu\n", errorCode);
			exit(-1);
		}
	}
}

void ThreadPool::EndThreadPool()
{
	IsFinish = true;
	for (size_t i = 0; i < MaxThreadNum; i++)
	{
		SetEvent(WorkerThreadEvent);
	}
	WaitForAllThreas();
}


