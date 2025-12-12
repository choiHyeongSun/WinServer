#pragma once
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/LogoutService.h"

class LogoutController : public IRecvMessage
{
	CONTROLLER_CLASS_H(LogoutController, EMESSAGE_TYPE::LOGOUT_PACKET)
	INJECTION(LogoutService, logoutService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;
};
