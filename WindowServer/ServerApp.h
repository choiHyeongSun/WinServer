#pragma once
#include "Server/BlockSocketWrapper.h"


class MemoryPool;
class ThreadPool;


__declspec(align(16))
struct UserRecvThreadParam
{
	UINT32 UserID = 0;
	BlockSocketWrapper* SockWrapper = nullptr;
};

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
	static bool isFinish;


	std::shared_ptr<BlockSocketWrapper> sockWrapper;
	HANDLE AcceptThread;

	friend DWORD WINAPI RecvProc(LPVOID LpParam);
	friend DWORD WINAPI AcceptProc(LPVOID LpParam);
};

