#include <MainPCH.h>
#include <Header/Server/UDPSocket.h>


void UDPSocket::InitSocket(addrinfo hint)
{
	Socket = socket(hint.ai_family, hint.ai_socktype, hint.ai_protocol);
	addrinfo* result = nullptr;
	INT32 iResult = getaddrinfo(nullptr, APP_PORT, &hint, &result);
	if (iResult != 0)
	{
		std::cout << "getAddrInfo failed : " << iResult;
		WSACleanup();
		exit(-1);
	}
	Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Socket == INVALID_SOCKET)
	{
		std::cout << "Socket failed : " << iResult;
		freeaddrinfo(result);
		WSACleanup();
		exit(-1);
	}

	iResult = bind(Socket, result->ai_addr, static_cast<INT32>(result->ai_addrlen));
	if (iResult != 0)
	{
		std::cout << "bind failed : " << iResult;
		freeaddrinfo(result);
		WSACleanup();
		exit(-1);
	}
}
