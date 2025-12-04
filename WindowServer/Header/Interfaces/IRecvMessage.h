#pragma once

class ISocketWrapper;
struct PacketHeader;

class IRecvMessage
{
public:
	virtual ~IRecvMessage() = default;
	virtual void OnRecvMessage(UINT32 userID,const PacketHeader* packetHeader , const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper) = 0;
};
