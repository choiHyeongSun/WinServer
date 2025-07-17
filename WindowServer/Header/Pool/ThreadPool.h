#pragma once

class ThreadPool
{

public:
	~ThreadPool();
	static std::shared_ptr<ThreadPool> CreateThreadPool(size_t InThreadMaxNum, size_t InJobMaxCapacity);

private:
	ThreadPool(size_t InThreadMaxNum, size_t InJobMaxCapacity);
	
private:
	size_t MaxThreadNum;
	size_t JobMaxCapacity;

	std::vector<HANDLE> Works;
	std::queue<std::function<void()>> Jobs;

	HANDLE Mutex;
	HANDLE WorkerThreadEvent;

	bool IsFinish;
public:
	bool SubmitJob(const std::function<void()>& InJob);
	void ExecutionThread();
	bool IsHasJobs() const;
	void WaitForAllThreas();
	void EndThreadPool();

private:
	std::function<void()> GetNextJob();

private:
	friend DWORD WINAPI ThreadProc(LPVOID LpParam);
};

