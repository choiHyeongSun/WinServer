#pragma once
#include "Controller.h"
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/ExitClientService.h"

class ExitClientController : public Controller, public IRecvMessage
{
	CONTROLLER_CLASS_H(ExitClientController, EMESSAGE_TYPE::EXIT_CLIENT);
	INJECTION(ExitClientService, exitClientService);

	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;
};
