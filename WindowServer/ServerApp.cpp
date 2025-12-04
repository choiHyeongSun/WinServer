#include "MainPCH.h"
#include "ServerApp.h"

#include "SendMsgTypes.h"
#include "Controllers/LoginController.h"
#include "Header/Pool/PoolingManager.h"
#include "Managers/ControllerManager.h"
#include "Managers/RecvManager.h"
#include "Pool/ThreadPool.h"

const size_t ServerApp::ThreadPoolMaxCount = 10;
const size_t ServerApp::ThreadPoolMaxJobCount = 300;

const size_t ServerApp::MemoryPoolSize = 1024ull * 1024ull;
const size_t ServerApp::MemoryPoolGroup = 1024ull;

bool ServerApp::isFinish = false;

DWORD WINAPI RecvProc(LPVOID LpParam)
{
	const INT32 bufferMaxSize = 1024;
	UserRecvThreadParam* recvThreadParam = static_cast<UserRecvThreadParam*>(LpParam);

	UINT32 userId = recvThreadParam->UserID;
	BlockSocketWrapper* sockWrapper = recvThreadParam->SockWrapper;
	while (!ServerApp::isFinish)
	{

		//builder 헤더 생성
		void* data = PacketBuilder::PacketBuild(EMESSAGE_TYPE::NONE);
		PoolingObject<PacketHeader>* header = static_cast<PoolingObject<PacketHeader>*>(data);

		//헤더 수신
		sockWrapper->RecvMessageHeader(userId, &header->Data);
		INT32 len = header->Data.MessageLen;
		if (len < 0)
		{
			std::cout << "Message Size is too Large. User ID : " << userId << ", Size : " << len << std::endl;
			break;
		}

		//전체 메시지 받아오기
		void* packet = PacketBuilder::PacketBuild(header->Data.MessageType);
		UINT32 totalRecvSize = 0;

		while (totalRecvSize < len)
		{
			int sub = len - totalRecvSize;
			sub = min(sub, bufferMaxSize);
			
			INT32 recvLen = sockWrapper->RecvMessage(userId, static_cast<char*>(packet) + totalRecvSize, sub);
			if (recvLen <= 0)
			{
				std::cout << "Recv Error User ID : " << userId << std::endl;
				break;
			}
			totalRecvSize += recvLen;
		}

		const char* totalData = static_cast<const char*>(packet);
		//Controller 실행
		RecvManager::ExecuteCallback(header->Data.MessageType, userId, &header->Data, totalData, len, sockWrapper);

		//메모리 해제
		PacketBuilder::ReleasePacket(header->Data.MessageType, &packet);
		PacketBuilder::ReleasePacket(EMESSAGE_TYPE::NONE, reinterpret_cast<void**>(&header));
	}
	sockWrapper->CloseSocket(userId);
	PoolingManager<UserRecvThreadParam>::FreeObject(&recvThreadParam);
	return 0;
}

DWORD WINAPI AcceptProc(LPVOID LpParam)
{
	BlockSocketWrapper* sockWrapper = static_cast<BlockSocketWrapper*>(LpParam);
	std::vector<HANDLE> recvThreads;

	while (!ServerApp::isFinish)
	{
		sockWrapper->AcceptClient([&](UINT32 userID)
		{
			std::cout << "Connected User ID : " << userID << std::endl;
			EntryServer entryMsg;
			entryMsg.MessageLen = sizeof(EntryServer);
			entryMsg.UserLocalId = userID;
			sockWrapper->PrivateMessage(userID, reinterpret_cast<const char*>(&entryMsg), sizeof(entryMsg), 0);

			UserRecvThreadParam* recvParam = PoolingManager<UserRecvThreadParam>::GenerateObject();
			recvParam->UserID = userID;
			recvParam->SockWrapper = sockWrapper;
			recvThreads.push_back(CreateThread(nullptr, 0, RecvProc, recvParam, 0, nullptr));
		});
	}

	WaitForMultipleObjects(recvThreads.size(), recvThreads.data(), TRUE, INFINITE);
	return 0;
}

ServerApp::ServerApp()
{
	ControllerManager::CreateInstance();
	ControllerManager::getInstance()->InitializeControllers();
	std::shared_ptr<ThreadPool> threadpool = ThreadPool::CreateThreadPool(ThreadPoolMaxCount, ThreadPoolMaxJobCount);

	threadpool->WaitForAllThreas();
	sockWrapper = BlockSocketWrapper::CreateSocketWrapper(PROTOCOL_TYPE::TCP, "Server Start.....");
	if (sockWrapper->getProtocolType() == PROTOCOL_TYPE::TCP)
	{
		AcceptThread = CreateThread(nullptr, 0, AcceptProc, sockWrapper.get(), 0, nullptr);
	}

	ControllerManager::DestroyInstance();
}

ServerApp::~ServerApp()
{
	LoginController::DestroyInstance();
	RecvManager::DestroyInstance();
}

INT32 ServerApp::Run()
{
	WaitForSingleObject(AcceptThread, INFINITE);
	return 0;
}



