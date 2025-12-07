#pragma once
#include <unordered_set>

#include "Sock.h"
#include "boost/functional/hash.hpp"
#include "Interfaces/ISocketWrapper.h"

struct PacketHeader;
class BlockingTCPSocket;
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


class BlockSocketWrapper : public ISocketWrapper
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
	static std::shared_ptr<BlockSocketWrapper> CreateSocketWrapper(PROTOCOL_TYPE protocol_type, std::string startMessage = "");

public:
	FORCEINLINE PROTOCOL_TYPE getProtocolType() { return ProtocolType; }

public:
	virtual void Broadcast(const char* msg, INT32 msgLen, INT32 flag) const override;
	virtual void GroupBroadcast(UINT32 groupID, const char* msg, INT32 msgLen, INT32 flag) override;
	virtual void PrivateMessage(UINT32 userID, const char* msg, INT32 msgLen, INT32 flag) override;
	virtual void AcceptClient(std::function<void(UINT32 userID)> connectCallback = nullptr) override;
	virtual INT32 RecvMessage(UINT32 userID, char* buffer, size_t bufferSize) override;
	virtual void RecvMessageHeader(UINT32 userID, PacketHeader* packetHeader) override;
	virtual void EntryGroup(UINT32 groupID, UINT32 userID) override;
	virtual void ExitGroup(UINT32 groupID, UINT32 userID) override;
	virtual bool IsGroupMember(UINT32 userId, UINT32 groupId) override;
	virtual void CloseSocket(UINT32 userID) override;


private:
	void InitSocketWrapper(PROTOCOL_TYPE protocol_type, std::string startMessage);

private:
	WSADATA Wsadata;


	std::shared_ptr<ListenSocket> ListenSock;
	std::shared_ptr<UDPSocket> UdpSock;

	std::unordered_map<UINT32, std::unordered_set<const INetworkSession*>> GroupNetworkSessions;
	std::unordered_map<UINT32, std::unordered_set<UDPClientInfo, UDPClinetInfoHasher>> GroupUdpClients;
	std::unordered_map<UINT32, const INetworkSession*> AllNetworkSessions;
	std::unordered_map<UINT32, UDPClientInfo> AllUdpClients;

	std::unordered_map<UINT32 ,std::shared_ptr<BlockingTCPSocket>> AllSockets;

	HANDLE NetworkSessionMutex;
	HANDLE UdpClientMutex;


	UINT32 LastUserID = 1;

	PROTOCOL_TYPE ProtocolType;
};

