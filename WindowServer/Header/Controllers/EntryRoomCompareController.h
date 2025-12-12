#pragma once
#include "Controller.h"
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Repositories/ChattingRoomRepository.h"
#include "Services/EntryRoomCompareService.h"

class EntryRoomCompareController : public Controller, public IRecvMessage
{
	CONTROLLER_CLASS_H(EntryRoomCompareController, EMESSAGE_TYPE::ENTRY_ROOM_COMPARE);
	INJECTION(EntryRoomCompareService, entryRoomCompareService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;

};
