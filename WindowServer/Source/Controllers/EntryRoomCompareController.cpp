#include "MainPCH.h"
#include "Header/Controllers/EntryRoomCompareController.h"

#include <string>

#include "Utility.h"
#include "Interfaces/ISocketWrapper.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(EntryRoomCompareController)

void EntryRoomCompareController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet,
	INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader);
	const EntryRoomComparePacket* entryRoomComparePacket =  reinterpret_cast<const EntryRoomComparePacket*>(packet);
	UINT32 roomId = htonl(entryRoomComparePacket->RoomId);

	std::string msg;
	if (socketWrapper->IsGroupMember(userID, roomId))
	{
		msg = "이미 입장한 채팅방입니다.";
		SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
		return;
	}

	msg = std::to_string(roomId);
	if (entryRoomCompareService->CompareRoom(roomId))
	{
		SendResult(socketWrapper, EMESSAGE_RESULT::SUCCESS, packetHeader, msg);
	}
	else
	{
		SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
	}

};
