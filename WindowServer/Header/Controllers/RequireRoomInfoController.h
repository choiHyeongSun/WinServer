#pragma once
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/RequireRoomInfoService.h"

class RequireRoomInfoController : public IRecvMessage
{
	CONTROLLER_CLASS_H(RequireRoomInfoController, EMESSAGE_TYPE::REQUIRE_ROOM_INFO)
	INJECTION(RequireRoomInfoService, requireRoomInfoService);
public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;
};
