#include "MainPCH.h"
#include "ServerApp.h"

#include "SendMsgTypes.h"
#include "Header/Pool/PoolingManager.h"
#include "Managers/ControllerManager.h"
#include "Managers/RecvManager.h"
#include "Pool/ThreadPool.h"
#include "mysql.h"
#include "Managers/DatabaseManager.h"
#include "Managers/RepositoryManager.h"
#include "Managers/ServiceManager.h"


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
		header->Data.PacketType = PacketBuilder::NtoHPacketType(header->Data.PacketType);
		header->Data.PacketLen = ntohl(header->Data.PacketLen);
		header->Data.UserLocalId = ntohl(header->Data.UserLocalId);

		INT32 len = header->Data.PacketLen;
		if (len < 0)
		{
			std::cout << "Message Size is too Large. User ID : " << userId << ", Size : " << len << std::endl;
			break;
		}

		//전체 메시지 받아오기
		std::vector<char> buffer(header->Data.PacketLen);
		UINT32 totalRecvSize = 0;

		while (totalRecvSize < len)
		{
			int sub = len - totalRecvSize;
			sub = min(sub, bufferMaxSize);
			
			INT32 recvLen = sockWrapper->RecvMessage(userId, buffer.data() + totalRecvSize, sub);
			if (recvLen <= 0)
			{
				std::cout << "Recv Error code : " << GetLastError() << std::endl;
				break;
			}
			totalRecvSize += recvLen;
		}
		//Controller 실행
		RecvManager::ExecuteCallback(header->Data.PacketType, userId, &header->Data, buffer.data(), len, sockWrapper);

		//메모리 해제
		PacketBuilder::ReleasePacket(EMESSAGE_TYPE::NONE, reinterpret_cast<void**>(&header));
	}
	sockWrapper->CloseSocket(userId);
	PoolingManager<UserRecvThreadParam>::FreeObject(&recvThreadParam);
	return 0;
}

//TCP 과정
DWORD WINAPI AcceptProc(LPVOID LpParam)
{
	BlockSocketWrapper* sockWrapper = static_cast<BlockSocketWrapper*>(LpParam);
	std::vector<HANDLE> recvThreads;

	while (!ServerApp::isFinish)
	{
		sockWrapper->AcceptClient([&](UINT32 userID)
		{
			DatabaseManager::getInstance()->RegisterId(userID);
			std::cout << "Connected User ID : " << userID << std::endl;
			EntryServer entryMsg;
			entryMsg.UserLocalId = htonl(userID);
			entryMsg.PacketType = PacketBuilder::HtonPacketType(entryMsg.PacketType);
			entryMsg.PacketLen = htonl(sizeof(entryMsg));


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

	DatabaseManager::CreateInstance();
	RepositoryManager::CreateInstance();
	ServiceManager::CreateInstance();
	ControllerManager::CreateInstance();

	DatabaseManager::getInstance()->Initialize("127.0.0.1", "root", "1234", "WindowServerDB", 3306);
	RepositoryManager::getInstance()->InitializeRepository();
	ServiceManager::getInstance()->InitializationService();
	ControllerManager::getInstance()->InitializeControllers();


	std::shared_ptr<ThreadPool> threadpool = ThreadPool::CreateThreadPool(ThreadPoolMaxCount, ThreadPoolMaxJobCount);

	threadpool->WaitForAllThreas();
	sockWrapper = BlockSocketWrapper::CreateSocketWrapper(PROTOCOL_TYPE::TCP, "Server Start.....");
	if (sockWrapper->getProtocolType() == PROTOCOL_TYPE::TCP)
	{
		AcceptThread = CreateThread(nullptr, 0, AcceptProc, sockWrapper.get(), 0, nullptr);
	}
}

ServerApp::~ServerApp()
{
	ControllerManager::DestroyInstance();
	DatabaseManager::DestroyInstance();
}

INT32 ServerApp::Run()
{
	WaitForSingleObject(AcceptThread, INFINITE);
	return 0;
}



