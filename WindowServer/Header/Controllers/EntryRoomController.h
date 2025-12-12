#pragma once
#include "Controller.h"
#include "Interfaces/IRecvMessage.h"
#include "Services/EntryRoomService.h"

class EntryRoomController : public Controller , public IRecvMessage
{
	CONTROLLER_CLASS_H(EntryRoomController, EMESSAGE_TYPE::ENTRY_ROOM);
	INJECTION(EntryRoomService, entryRoomService);
	
public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;

	void SendEntry(UINT32 userID, INT32 roomID, ISocketWrapper* socketWrapper);
};
