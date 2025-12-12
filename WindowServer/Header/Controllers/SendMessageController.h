#pragma once
#include "Controller.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/SendMessageService.h"

class SendMessageController : public Controller, public IRecvMessage
{
	CONTROLLER_CLASS_H(SendMessageController, EMESSAGE_TYPE::SEND_MESSAGE);
	INJECTION(SendMessageService, sendMessageService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;
};
