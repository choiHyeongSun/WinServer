#include "MainPCH.h"
#include "Header/Server/BlockSocketWrapper.h"
#include "Header/Server/ListenSocket.h"
#include "Header/Server/UDPSocket.h"
#include "Header/Interfaces/INetworkSession.h"
#include "Server/BlockingSock.h"

std::shared_ptr<BlockSocketWrapper> BlockSocketWrapper::CreateSocketWrapper(PROTOCOL_TYPE protocol_type)
{
	std::shared_ptr<BlockSocketWrapper> createWrapper = std::shared_ptr<BlockSocketWrapper>(new BlockSocketWrapper());
	createWrapper->InitSocketWrapper(protocol_type);
	return createWrapper;
}

void BlockSocketWrapper::InitSocketWrapper(PROTOCOL_TYPE protocol_type)
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

	if (protocol_type == PROTOCOL_TYPE::UDP)
	{
		hint.ai_socktype = SOCK_DGRAM;
		hint.ai_protocol = IPPROTO_UDP;
		UdpSock = Sock<UDPSocket>::GeneratedSocket(hint);
	}

	ProtocolType = protocol_type;
}

void BlockSocketWrapper::AcceptClient(std::function<void(UINT32 userID)> connectCallback)
{
	if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		std::cout << "Warning : UDP Socket does not support AcceptClient()" << std::endl;
		return;
	}


	std::shared_ptr<BlockingSocket> blockingSock = Sock<BlockingSocket>::GeneratedSocket();
	blockingSock->AcceptSocket(ListenSock);
	
	AllNetworkSessions.insert({ LastUserID++, blockingSock.get() });
	if (connectCallback != nullptr)
	{
		connectCallback(LastUserID);
	}
}


void BlockSocketWrapper::Broadcast(void* msg, INT32 msgLen, INT32 flag)
{
	if (ProtocolType == PROTOCOL_TYPE::TCP) {
		for (const auto& session : AllNetworkSessions)
		{
			session.second->SendMsg(msg, msgLen, flag);
		}
	}
	else if (ProtocolType == PROTOCOL_TYPE::UDP) {
		for (const auto& clientInfo : AllUdpClients)
		{
			const UDPClientInfo& client = clientInfo.second;
			const char* sendMsg = reinterpret_cast<const char*>(msg);
			sendto(UdpSock->getSocket(), sendMsg, msgLen, flag, client.ClientAddr, client.AddrLen);
		}
	}
}

void BlockSocketWrapper::GroupBroadcast(INT32 groupID, void* msg, INT32 msgLen, INT32 flag)
{
	if (ProtocolType == PROTOCOL_TYPE::TCP)
	{
		for (const INetworkSession* session : GroupNetworkSessions[groupID])
		{
			session->SendMsg(msg, msgLen, flag);
		}
	}
	else if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		for (const UDPClientInfo& client : GroupUdpClients[groupID])
		{
			const char* sendMsg = reinterpret_cast<const char*>(msg);
			sendto(UdpSock->getSocket(), sendMsg, msgLen, flag, client.ClientAddr, client.AddrLen);
		}
	}
}

void BlockSocketWrapper::PrivateMessage(INT32 userID, void* msg, INT32 msgLen, INT32 flag)
{
	if (ProtocolType == PROTOCOL_TYPE::TCP)
	{
		if (AllNetworkSessions.find(userID) == AllNetworkSessions.cend())
		{
			return;
		}
		INetworkSession* session = AllNetworkSessions[userID];
		session->SendMsg(msg, msgLen, flag);
	}
	else if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		if (AllUdpClients.find(userID) == AllUdpClients.cend())
		{
			return;
		}
		UDPClientInfo& client = AllUdpClients[userID];
		const char* sendMsg = reinterpret_cast<const char*>(msg);
		sendto(UdpSock->getSocket(), sendMsg, msgLen, flag, client.ClientAddr, client.AddrLen);
	}
}

bool BlockSocketWrapper::IsGroupMember(UINT32 userId, UINT32 groupId)
{
	if (ProtocolType == PROTOCOL_TYPE::TCP)
	{
		if (AllNetworkSessions.find(userId) == AllNetworkSessions.cend() ||
			GroupNetworkSessions.find(groupId) == GroupNetworkSessions.cend())
		{
			return false;
		}

		INetworkSession* session = AllNetworkSessions[userId];
		return GroupNetworkSessions[groupId].find(session) != GroupNetworkSessions[groupId].cend();
	}
	else if (ProtocolType == PROTOCOL_TYPE::UDP)
	{
		if (AllUdpClients.find(userId) == AllUdpClients.cend() ||
			GroupUdpClients.find(groupId) == GroupUdpClients.cend())
		{
			return false;
		}

		const UDPClientInfo& session = AllUdpClients[userId];
		return GroupUdpClients[groupId].find(session) != GroupUdpClients[groupId].cend();
	}
	return false;
}

