#pragma once
#include "Sock.h"

class ListenSocket;

class BlockingSocket : public Sock<BlockingSocket>
{
public:
	BlockingSocket()
	{
	}
	virtual ~BlockingSocket() {
	}


public:
	void AcceptSocket(std::shared_ptr<ListenSocket> listenSock);

private:
	SOCKADDR ClientAddr = { 0 };
	INT32 ClientAddrLen = 0;
};
