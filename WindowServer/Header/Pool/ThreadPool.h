#pragma once

class ThreadPool
{

public:
	~ThreadPool();
	static std::shared_ptr<ThreadPool> CreateThreadPool(size_t InThreadMaxNum, size_t InJobMaxCapacity);

private:
	ThreadPool(size_t InThreadMaxNum, size_t InJobMaxCapacity);

public:
	bool SubmitJob(const std::function<void()>& InJob);
	bool IsHasJobs() const;
	void WaitForAllThreas();
	void EndThreadPool();

private:
	std::function<void()> GetNextJob();
	
private:
	size_t MaxThreadNum;
	size_t JobMaxCapacity;
	size_t WorkingThreadCount;

	std::vector<HANDLE> Works;
	std::queue<std::function<void()>> Jobs;

	HANDLE Mutex;
	HANDLE WorkerThreadSemaphore;
	HANDLE WorkingEvent;


	bool IsFinish;

private:
	friend DWORD WINAPI ThreadProc(LPVOID LpParam);
};

