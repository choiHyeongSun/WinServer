#pragma once

class INetworkSession
{
public:
	virtual void SendMsg(void* msg, int msgLen, int flag) const { }
	virtual void SendMsg(SOCKADDR* sock, void* msg, int msgLen, int flag) const {}
	virtual void* RecvMsg() const { return nullptr; }
};