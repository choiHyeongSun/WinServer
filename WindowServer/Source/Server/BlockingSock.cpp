#include "MainPCH.h"
#include "Header/Server/BlockingSock.h"

#include <WS2tcpip.h>

#include "Server/ListenSocket.h"

void BlockingSocket::AcceptSocket(std::shared_ptr<ListenSocket> listenSock)
{
	ZeroMemory(&ClientAddr, sizeof(SOCKADDR));
	Socket = accept(listenSock->getSocket(), &ClientAddr, &ClientAddrLen);
	if (Socket == INVALID_SOCKET)
	{
		std::cout << "accept failed : " << WSAGetLastError();
		exit(-1);
	}
}
