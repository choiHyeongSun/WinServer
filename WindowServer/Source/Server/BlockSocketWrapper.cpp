#include "MainPCH.h"
#include "Header/Server/BlockSocketWrapper.h"
#include "Header/Server/ListenSocket.h"
#include "Header/Server/UDPSocket.h"
#include "Header/Interfaces/INetworkSession.h"
#include "Header/SendMsgTypes.h"
#include "Server/BlockingSock.h"


std::shared_ptr<BlockSocketWrapper> BlockSocketWrapper::CreateSocketWrapper(PROTOCOL_TYPE protocol_type, std::string startMessage)
{
	std::shared_ptr<BlockSocketWrapper> createWrapper = std::shared_ptr<BlockSocketWrapper>(new BlockSocketWrapper());
	createWrapper->InitSocketWrapper(protocol_type, startMessage);
	return createWrapper;
}

void BlockSocketWrapper::InitSocketWrapper(PROTOCOL_TYPE protocol_type, std::string startMessage)
{
	INT32 iResult = WSAStartup(MAKEWORD(2, 2), &Wsadata);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed: " << iResult;
		exit(-1);
	}
	addrinfo hint;
	ZeroMemory(&hint, sizeof(hint));

	hint.ai_family = AF_INET;
	hint.ai_flags = AI_PASSIVE;
	if (protocol_type == PROTOCOL_TYPE::TCP)
	{
		hint.ai_socktype = SOCK_STREAM;
		hint.ai_protocol = IPPROTO_TCP;
		ListenSock = Sock<ListenSocket>::GeneratedSocket(hint);
	}

	else if (protocol_type == PROTOCOL_TYPE::UDP)
	{
		hint.ai_socktype = SOCK_DGRAM;
		hint.ai_protocol = IPPROTO_UDP;
		UdpSock = Sock<UDPSocket>::GeneratedSocket(hint);
	}

	NetworkSessionMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (NetworkSessionMutex == nullptr)
	{
		std::cout << "NetworkSessionMutex is nullptr" << std::endl;
		exit(-1);
	}

	UdpClientMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (UdpClientMutex == nullptr)
	{
		std::cout << "UdpClientMutex is nullptr" << std::endl;
		exit(-1);
	}

	std::cout << startMessage << std::endl;
	ProtocolType = protocol_type;

}

void BlockSocketWrapper::AcceptClient(std::function<void(UINT32 userID)> connectCallback)
{
	if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		std::cout << "Warning : UDP Socket does not support AcceptClient()" << std::endl;
		return;
	}

	std::shared_ptr<BlockingTCPSocket> blockingSock = Sock<BlockingTCPSocket>::GeneratedSocket();
	blockingSock->AcceptSocket(ListenSock);


	MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
	AllNetworkSessions.insert({ LastUserID, blockingSock.get() });
	AllSockets.insert({ LastUserID, blockingSock });
	ReleaseMutex(NetworkSessionMutex);

	if (connectCallback != nullptr)
	{
		connectCallback(LastUserID);
	}
	LastUserID++;
}

void BlockSocketWrapper::CloseSocket(UINT32 userID)
{
	if (ProtocolType == PROTOCOL_TYPE::TCP)
	{
		MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
		for (auto& group : GroupNetworkSessions)
		{
			group.second.erase(AllNetworkSessions[userID]);
		}

		AllNetworkSessions.erase(userID);
		AllSockets.erase(userID);
		ReleaseMutex(NetworkSessionMutex);
	}

	if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
		for (auto& group : GroupUdpClients)
		{
			group.second.erase(AllUdpClients[userID]);
		}

		AllUdpClients.erase(userID);
		ReleaseMutex(UdpClientMutex);
	}
}

INT32 BlockSocketWrapper::CountGroup(UINT32 groupID)
{
	if (ProtocolType == PROTOCOL_TYPE::TCP)
	{
		MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
		if (GroupNetworkSessions.find(groupID) != GroupNetworkSessions.cend())
		{
			INT32 size = GroupNetworkSessions[groupID].size();
			ReleaseMutex(NetworkSessionMutex);
			return size;
		}
		ReleaseMutex(NetworkSessionMutex);
		return -1;
	}
	if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
		if (GroupUdpClients.find(groupID) != GroupUdpClients.cend())
		{
			INT32 size = GroupUdpClients[groupID].size();
			ReleaseMutex(UdpClientMutex);
			return size;
		}
		ReleaseMutex(UdpClientMutex);
		return -1;
	}
	return -1;

}

INT32 BlockSocketWrapper::RecvMessage(UINT32 userID, char* buffer, size_t bufferSize)
{
	MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
	if (AllNetworkSessions.find(userID) == AllNetworkSessions.cend())
	{
		ReleaseMutex(NetworkSessionMutex);
		std::cout << "User를 찾을 수 없습니다.";
		return 0;
	}
	const INetworkSession* networkSession = AllNetworkSessions[userID];
	ReleaseMutex(NetworkSessionMutex);

	return networkSession->RecvMsg(buffer, bufferSize, 0);
}

void BlockSocketWrapper::RecvMessageHeader(UINT32 userID, PacketHeader* packetHeader)
{
	MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
	if (AllNetworkSessions.find(userID) == AllNetworkSessions.cend())
	{
		ReleaseMutex(NetworkSessionMutex);
		std::cout << "User를 찾을 수 없습니다.";
		packetHeader->PacketLen = -1;
		return;
	}
	const INetworkSession* networkSession = AllNetworkSessions[userID];
	ReleaseMutex(NetworkSessionMutex);

	INT32 recv = networkSession->RecvMsg(reinterpret_cast<char*>(packetHeader), sizeof(PacketHeader), MSG_PEEK);
	if (recv <= 0)
	{
		std::cout << "recv error : " << GetLastError() << std::endl;
		packetHeader->PacketLen = -1;
		return;
	}
}

void BlockSocketWrapper::EntryGroup(UINT32 groupID, UINT32 userID)
{
	switch (ProtocolType)
	{
	case PROTOCOL_TYPE::TCP:
	{
		MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
		if (AllNetworkSessions.find(userID) == AllNetworkSessions.cend())
		{
			ReleaseMutex(NetworkSessionMutex);
			return;
		}

		const INetworkSession* network = AllNetworkSessions[userID];
		if (GroupNetworkSessions[groupID].find(network) == GroupNetworkSessions[groupID].cend())
			GroupNetworkSessions[groupID].insert(network);
		ReleaseMutex(NetworkSessionMutex);
		break;
	}

	case PROTOCOL_TYPE::UDP:
		MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
		if (AllUdpClients.find(userID) == AllUdpClients.cend())
		{
			ReleaseMutex(UdpClientMutex);
			return;
		}

		UDPClientInfo ClientInfp = AllUdpClients[userID];
		if (GroupUdpClients[groupID].find(ClientInfp) != GroupUdpClients[groupID].cend())
			GroupUdpClients[groupID].insert(ClientInfp);
		ReleaseMutex(UdpClientMutex);
		break;
	}
}

void BlockSocketWrapper::ExitGroup(UINT32 groupID, UINT32 userID)
{
	switch (ProtocolType)
	{
	case PROTOCOL_TYPE::TCP:
		if (IsGroupMember(userID, groupID))
		{
			MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
			GroupNetworkSessions[groupID].erase(AllNetworkSessions[userID]);
			ReleaseMutex(NetworkSessionMutex);
		}
		break;

	case PROTOCOL_TYPE::UDP:
		if (IsGroupMember(userID, groupID))
		{
			MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
			UDPClientInfo ClientInfp = AllUdpClients[userID];
			GroupUdpClients[groupID].erase(ClientInfp);
			ReleaseMutex(UdpClientMutex);
		}
		break;
	}
}

void BlockSocketWrapper::Broadcast(const char* msg, INT32 msgLen, INT32 flag) const
{
	switch (ProtocolType)
	{
	case PROTOCOL_TYPE::TCP:
		MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
		for (const auto& session : AllNetworkSessions)
		{
			session.second->SendMsg(msg, msgLen, flag);
		}
		ReleaseMutex(NetworkSessionMutex);
		break;


	case PROTOCOL_TYPE::UDP:
		MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
		for (const auto& clientInfo : AllUdpClients)
		{
			const UDPClientInfo& client = clientInfo.second;
			const char* sendMsg = reinterpret_cast<const char*>(msg);
			sendto(UdpSock->getSocket(), sendMsg, msgLen, flag, client.ClientAddr, client.AddrLen);
		}
		ReleaseMutex(UdpClientMutex);
		break;
	}
}

void BlockSocketWrapper::GroupBroadcast(UINT32 groupID, const char* msg, INT32 msgLen, INT32 flag)
{

	switch (ProtocolType)
	{
	case PROTOCOL_TYPE::TCP:
		for (const INetworkSession* session : GroupNetworkSessions[groupID])
		{
			session->SendMsg(msg, msgLen, flag);
		}
		break;

	case PROTOCOL_TYPE::UDP:
		for (const UDPClientInfo& client : GroupUdpClients[groupID])
		{
			const char* sendMsg = reinterpret_cast<const char*>(msg);
			sendto(UdpSock->getSocket(), sendMsg, msgLen, flag, client.ClientAddr, client.AddrLen);
		}
		break;
	}
}

void BlockSocketWrapper::PrivateMessage(UINT32 userID, const char* msg, INT32 msgLen, INT32 flag)
{
	switch (ProtocolType)
	{
	case PROTOCOL_TYPE::TCP:
	{
		MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
		if (AllNetworkSessions.find(userID) == AllNetworkSessions.cend())
		{
			ReleaseMutex(NetworkSessionMutex);
			return;
		}
		const INetworkSession* session = AllNetworkSessions[userID];
		ReleaseMutex(NetworkSessionMutex);
		session->SendMsg(msg, msgLen, flag);
		break;
	}

	case PROTOCOL_TYPE::UDP:
	{
		MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
		if (AllUdpClients.find(userID) == AllUdpClients.cend())
		{
			return;
		}
		const UDPClientInfo& client = AllUdpClients[userID];
		ReleaseMutex(UdpClientMutex);
		const char* sendMsg = reinterpret_cast<const char*>(msg);
		sendto(UdpSock->getSocket(), sendMsg, msgLen, flag, client.ClientAddr, client.AddrLen);
		break;
	}
	}
}

bool BlockSocketWrapper::IsGroupMember(UINT32 userId, UINT32 groupId)
{
	switch (ProtocolType)
	{
	case PROTOCOL_TYPE::TCP:
	{
		MUTEX_CHECK(WaitForSingleObject(NetworkSessionMutex, INFINITE));
		if (AllNetworkSessions.find(userId) == AllNetworkSessions.cend() ||
			GroupNetworkSessions.find(groupId) == GroupNetworkSessions.cend())
		{
			ReleaseMutex(NetworkSessionMutex);
			return false;
		}

		const INetworkSession* session = AllNetworkSessions[userId];
		bool result = GroupNetworkSessions[groupId].find(session) != GroupNetworkSessions[groupId].cend();
		ReleaseMutex(NetworkSessionMutex);
		return result;
	}

	case PROTOCOL_TYPE::UDP:
	{
		MUTEX_CHECK(WaitForSingleObject(UdpClientMutex, INFINITE));
		if (AllUdpClients.find(userId) == AllUdpClients.cend() ||
			GroupUdpClients.find(groupId) == GroupUdpClients.cend())
		{
			ReleaseMutex(UdpClientMutex);
			return false;
		}
		const UDPClientInfo& session = AllUdpClients[userId];
		ReleaseMutex(UdpClientMutex);
		bool result = GroupUdpClients[groupId].find(session) != GroupUdpClients[groupId].cend();
		return result;
	}
	}
	return false;
}