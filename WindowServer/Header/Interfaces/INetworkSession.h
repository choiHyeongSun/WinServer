#pragma once

class INetworkSession
{
public:
	virtual void SendMsg(const char* msg, INT32 msgLen, INT32 flag) const { }
	virtual void SendMsg(SOCKADDR* sock, void* msg, INT32 msgLen, INT32 flag) const {}
	virtual INT32 RecvMsg(char* buffer, size_t bufferSize, INT32 flag) const { return 0; }
};