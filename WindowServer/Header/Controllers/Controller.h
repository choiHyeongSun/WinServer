#pragma once
#include "SendMsgTypes.h"

class ISocketWrapper;

class Controller
{
protected:
	void SendResult(ISocketWrapper* socketWrapper, EMESSAGE_RESULT resultType, const PacketHeader* packetHeader, std::string msg);
	void GetResult(EMESSAGE_RESULT resultType, const PacketHeader* packetHeader, std::string msg, PoolingObject<ResultPacket>* outResult);
};


