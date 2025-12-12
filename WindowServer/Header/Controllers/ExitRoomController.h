#pragma once
#include "Controller.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/ExitRoomService.h"

class ExitRoomController : public Controller, public IRecvMessage
{
	CONTROLLER_CLASS_H(ExitRoomController, EMESSAGE_TYPE::EXIT_ROOM);
	INJECTION(ExitRoomService, exitRoomService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;

	void SendExit(UINT32 userID, INT32 roomID, ISocketWrapper* socketWrapper);
};
