#pragma once
#include <unordered_set>

#include "Sock.h"
#include "boost/functional/hash.hpp"

class INetworkSession;
class ListenSocket;
class UDPSocket;

enum class SOCKET_TYPE : UINT8
{
	BLOCKING = 0,
	NON_BLOCKING
};
enum class PROTOCOL_TYPE : UINT8
{
	TCP = 0,
	UDP
};


class BlockSocketWrapper
{
private:
	struct UDPClientInfo
	{
		SOCKADDR* ClientAddr = nullptr;
		INT32 AddrLen = 0;

		bool operator== (const UDPClientInfo& other) const
		{
			if (AddrLen != other.AddrLen)
				return false;
			if (ClientAddr == nullptr || other.ClientAddr == nullptr)
				return ClientAddr == other.ClientAddr;

			return memcmp(ClientAddr, other.ClientAddr, AddrLen) == 0;
		}
	};

	struct UDPClinetInfoHasher
	{
		std::size_t operator()(const UDPClientInfo& clientInfo) const
		{
			const char* start = reinterpret_cast<const char*>(clientInfo.ClientAddr);
			const char* end = start + clientInfo.AddrLen;

			return boost::hash_range(start, end);
		}
	};

private:
	BlockSocketWrapper() {}

public:
	static std::shared_ptr<BlockSocketWrapper> CreateSocketWrapper(PROTOCOL_TYPE protocol_type);


public:
	void Broadcast(void* msg, INT32 msgLen, INT32 flag);
	void GroupBroadcast(INT32 groupID, void* msg, INT32 msgLen, INT32 flag);
	void PrivateMessage(INT32 userID, void* msg, INT32 msgLen, INT32 flag);

private:
	void InitSocketWrapper(PROTOCOL_TYPE protocol_type);
	void AcceptClient(std::function<void(UINT32 userID)> connectCallback = nullptr);
	bool IsGroupMember(UINT32 userId, UINT32 groupId);
	

private:
	WSADATA Wsadata;

	PROTOCOL_TYPE ProtocolType;

	std::shared_ptr<ListenSocket> ListenSock;
	std::shared_ptr<UDPSocket> UdpSock;

	std::unordered_map<UINT32, std::unordered_set<INetworkSession*>> GroupNetworkSessions;
	std::unordered_map<UINT32, std::unordered_set<UDPClientInfo, UDPClinetInfoHasher>> GroupUdpClients;
	std::unordered_map<UINT32, INetworkSession*> AllNetworkSessions;
	std::unordered_map<UINT32, UDPClientInfo> AllUdpClients;

	UINT32 LastUserID= 0;
};

