#pragma once

struct PacketHeader;

class ISocketWrapper
{
public:
	virtual void Broadcast(const char* msg, INT32 msgLen, INT32 flag) const = 0;
	virtual void GroupBroadcast(UINT32 groupID, const char* msg, INT32 msgLen, INT32 flag) = 0;
	virtual void PrivateMessage(UINT32 userID, const char* msg, INT32 msgLen, INT32 flag) = 0;
	virtual void AcceptClient(std::function<void(UINT32 userID)> connectCallback = nullptr) = 0;
	virtual INT32 RecvMessage(UINT32 userID, char* buffer, size_t bufferSize) = 0;
	virtual void RecvMessageHeader(UINT32 userID, PacketHeader* packetHeader) = 0;
	virtual void EntryGroup(UINT32 groupID, UINT32 userID) = 0;
	virtual void ExitGroup(UINT32 groupID, UINT32 userID) = 0;
	virtual bool IsGroupMember(UINT32 userId, UINT32 groupId) = 0;
	virtual void CloseSocket(UINT32 userID) = 0;
	virtual INT32 CountGroup(UINT32 groupID) = 0;
};
