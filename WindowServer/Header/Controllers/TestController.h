#pragma once
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Managers/RecvManager.h"

class TestController : public IRecvMessage
{
	CONTROLLER_CLASS_H(TestController, EMESSAGE_TYPE::TEST_PACKET)
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper) override;;
};
