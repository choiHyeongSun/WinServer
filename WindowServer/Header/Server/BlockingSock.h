#pragma once
#include "Sock.h"

class ListenSocket;

class BlockingTCPSocket : public Sock<BlockingTCPSocket>
{
public:
	BlockingTCPSocket()
	{
	}
	virtual ~BlockingTCPSocket() {
	}

public:
	virtual void SendMsg(const char* msg, INT32 msgLen, INT32 flag) const override;
	virtual INT32 RecvMsg(char* buffer, size_t bufferSize, INT32 flag) const override;

public:
	void AcceptSocket(const std::shared_ptr<ListenSocket>& listenSock);


private:
	SOCKADDR ClientAddr = { 0 };
	INT32 ClientAddrLen = 0;
};
