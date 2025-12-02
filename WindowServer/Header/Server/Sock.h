#pragma once
#include "Interfaces/INetworkSession.h"

template<class TYPE>
class Sock : public INetworkSession
{
public:
	Sock() {}
	~Sock() {
		closesocket(Socket);
	}

public:
	static std::shared_ptr<TYPE> GeneratedSocket(addrinfo hint);
	static std::shared_ptr<TYPE> GeneratedSocket();


public:
	virtual void InitSocket(addrinfo hint) {}

public:
	FORCEINLINE SOCKET getSocket() const { return Socket; }

protected:
	SOCKET Socket = 0;
};

template <class TYPE>
std::shared_ptr<TYPE> Sock<TYPE>::GeneratedSocket(addrinfo hint)
{
	std::shared_ptr<TYPE> sock = std::shared_ptr<TYPE>(new TYPE());
	if (sock == nullptr)
	{
		std::cout << "GeneratedSocket Error" << std::endl;
	}
	sock->InitSocket(hint);
	return sock;
}

template <class TYPE>
std::shared_ptr<TYPE> Sock<TYPE>::GeneratedSocket()
{
	std::shared_ptr<TYPE> sock = std::shared_ptr<TYPE>(new TYPE());
	if (sock == nullptr)
	{
		std::cout << "GeneratedSocket Error" << std::endl;
	}
	return sock;
}
