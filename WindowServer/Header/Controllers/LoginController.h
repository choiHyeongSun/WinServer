#pragma once
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"

class LoginController : public IRecvMessage
{
private:
	CONTROLLER_CLASS_H(LoginController, EMESSAGE_TYPE::LOGIN_PACKET)

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper) override;
};
