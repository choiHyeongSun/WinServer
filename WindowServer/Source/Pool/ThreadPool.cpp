#include "MainPCH.h"
#include "Pool/ThreadPool.h"

DWORD WINAPI ThreadProc(LPVOID LpParam)
{
	ThreadPool* threadPool = static_cast<ThreadPool*>(LpParam);
	if (threadPool == nullptr)
		return 1;

	while (true)
	{
		DWORD dwWaitResult = WaitForSingleObject(threadPool->WorkerThreadSemaphore, INFINITE);
		if (dwWaitResult == WAIT_FAILED)
		{
			DWORD errorCode = GetLastError();
			printf("Wait failed. Error code: %lu\n", errorCode);
			exit(-1);
		}
		if (threadPool->IsFinish) break;

		std::function<void()> callback = threadPool->GetNextJob();
		if (callback != nullptr)
		{
			callback();
		}
		InterlockedDecrement(&threadPool->WorkingThreadCount);
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
	MaxThreadNum(InThreadMaxNum), JobMaxCapacity(InJobMaxCapacity), IsFinish(false), WorkingThreadCount(0)
{
	WorkingEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
	WorkerThreadSemaphore = CreateSemaphore(nullptr, 0, InThreadMaxNum, nullptr);
	if (WorkerThreadSemaphore == nullptr)
	{
		std::cout << "Event Create Failed" << std::endl;
		exit(-1);
	}

	for (size_t i = 0; i < MaxThreadNum; i++)
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
	CloseHandle(WorkerThreadSemaphore);
	for (HANDLE workThread : Works)
	{
		CloseHandle(workThread);
	}
}

bool ThreadPool::SubmitJob(const std::function<void()>& InJob)
{
	DWORD dwWaitResult = WaitForSingleObject(Mutex, INFINITE);
	if (dwWaitResult == WAIT_FAILED)
	{
		DWORD errorCode = GetLastError();
		printf("Wait failed. Error code: %lu\n", errorCode);
		exit(-1);
	}

	if (Jobs.size() >= JobMaxCapacity)
	{
		ReleaseMutex(Mutex);
		return false;
	}

	InterlockedIncrement(&WorkingThreadCount);
	Jobs.push(InJob);
	SetEvent(WorkingEvent);
	ReleaseSemaphore(WorkerThreadSemaphore, 1, nullptr);
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
		ReleaseMutex(Mutex);
		return nullptr;
	}

	std::function<void()> callback = Jobs.front(); Jobs.pop();
	ReleaseMutex(Mutex);
	return callback;
}

bool ThreadPool::IsHasJobs() const
{
	return !Jobs.empty();
}

void ThreadPool::WaitForAllThreas()
{
	while (WorkingThreadCount != 0)
	{
		Sleep(1);
	}
}

void ThreadPool::EndThreadPool()
{
	IsFinish = true;
	ReleaseSemaphore(WorkerThreadSemaphore, MaxThreadNum, nullptr);
	WaitForAllThreas();
}


