#include "MainPCH.h"
#include "Header/Server/BlockingSock.h"

#include <WS2tcpip.h>

#include "Server/ListenSocket.h"

void BlockingTCPSocket::AcceptSocket(const std::shared_ptr<ListenSocket>& listenSock)
{
	ZeroMemory(&ClientAddr, sizeof(SOCKADDR));
	ClientAddrLen = sizeof(ClientAddr);

	Socket = accept(listenSock->getSocket(), &ClientAddr, &ClientAddrLen);
	if (Socket == INVALID_SOCKET)
	{
		std::cout << "accept failed : " << WSAGetLastError();
		exit(-1);
	}
}

void BlockingTCPSocket::SendMsg(const char* msg, INT32 msgLen, INT32 flag) const
{
	Sock::SendMsg(msg, msgLen, flag);
	int totalMsg = 0;

	while (totalMsg < msgLen)
	{
		int sendLen = msgLen - totalMsg;
		int len = send(Socket, msg + totalMsg, sendLen, flag);
		totalMsg += len;
	}
}

INT32 BlockingTCPSocket::RecvMsg(char* buffer, size_t bufferSize, INT32 flag) const
{
	Sock::RecvMsg(buffer, bufferSize, flag);
	return recv(Socket, buffer, bufferSize, flag);
}
