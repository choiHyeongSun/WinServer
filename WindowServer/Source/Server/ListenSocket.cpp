#include "MainPCH.h"
#include "Header/Server/ListenSocket.h"

void ListenSocket::InitSocket(addrinfo hint)
{
	addrinfo* result = nullptr;
	INT32 iResult = getaddrinfo(nullptr, APP_PORT, &hint, &result);
	
	if (iResult != 0)
	{
		std::cout << "getAddrInfo failed : " << iResult;
		WSACleanup();
		exit(-1);
	}
	std::cout << "INFO: getaddrinfo Success" << std::endl;
	
	Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Socket == INVALID_SOCKET)
	{
		std::cout << "Socket failed : " << iResult;
		freeaddrinfo(result);
		WSACleanup();
		exit(-1);
	}

	iResult = bind(Socket, result->ai_addr, result->ai_addrlen);
	if (iResult != 0)
	{
		std::cout << "getAddrInfo failed : " << iResult;
		freeaddrinfo(result);
		WSACleanup();
		exit(-1);
	}
	
	
	iResult = listen(Socket, MAX_LISTEN_COUNT);
	if (iResult != 0)
	{
		std::cout << "getAddrInfo failed : " << iResult;
		freeaddrinfo(result);
		WSACleanup();
		exit(-1);
	}

	std::cout << "INFO: Listen Socket Success" << std::endl;
	ServerAddr = result;
}
