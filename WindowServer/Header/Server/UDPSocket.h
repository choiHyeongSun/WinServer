#pragma once
#include "Sock.h"

class UDPSocket : public Sock<UDPSocket>
{
public:
	UDPSocket() {}
	virtual ~UDPSocket() {
		freeaddrinfo(ServerAddr);
	}

public:
	void InitSocket(addrinfo hint) override;

private:
	addrinfo* ServerAddr = nullptr;
};

