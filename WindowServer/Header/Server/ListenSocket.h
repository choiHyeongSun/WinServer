#pragma once
#include "Sock.h"

class ListenSocket : public Sock<ListenSocket>
{
public:
	ListenSocket() {}
	virtual ~ListenSocket()
	{
		freeaddrinfo(ServerAddr);
	}

public:
	virtual void InitSocket(addrinfo hint) override;

private:
	addrinfo* ServerAddr = nullptr;
};