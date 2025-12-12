#pragma once
#include "Controller.h"
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/CreateChattingRoomService.h"

class CreateChattingRoomController : public Controller ,public IRecvMessage
{
	CONTROLLER_CLASS_H(CreateChattingRoomController, EMESSAGE_TYPE::CREATE_ROOM)
	INJECTION(CreateChattingRoomService, createChattingRoomService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;
};
