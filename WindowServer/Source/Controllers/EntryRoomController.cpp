#include "MainPCH.h"
#include "Header/Controllers/EntryRoomController.h"

#include <string>

#include "Utility.h"
#include "Header/Managers/RecvManager.h"
#include "Interfaces/ISocketWrapper.h"
#include "Managers/DatabaseManager.h"

CONTROLLER_CLASS_CPP(EntryRoomController)

void EntryRoomController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
                                        ISocketWrapper* socketWrapper)
{
	const EntryRoomPacket* entryRoomPacket = reinterpret_cast<const EntryRoomPacket*>(packet);
	INT32 roomid = ntohl(entryRoomPacket->RoomId);
	std::string password = Utility::ToANSI(entryRoomPacket->Password);
	std::string msg = "";
	if (socketWrapper->IsGroupMember(userID, roomid))
	{
		return;
	}
	INT32 dbUserID = DatabaseManager::getInstance()->GetUserID(userID);
	if (entryRoomService->PasswordCompare(dbUserID, roomid, password))
	{
		msg = std::to_string(roomid);
		SendEntry(userID, roomid, socketWrapper);
		socketWrapper->EntryGroup(roomid, userID);
		SendResult(socketWrapper, EMESSAGE_RESULT::SUCCESS, packetHeader, msg);
	}
	else
	{
		msg = "비밀번호가 맞지 않습니다.";
		SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
	}
}

void EntryRoomController::SendEntry(UINT32 userID, INT32 roomID, ISocketWrapper* socketWrapper)
{
	PoolingObject<EntryMemberPacket>* packet = PacketBuilder::PacketBuild<EntryMemberPacket>();
	INT32 dbUserID = DatabaseManager::getInstance()->GetUserID(userID);

	INT32 totalPacketSize = sizeof(EntryMemberPacket) + sizeof(RoomMember);
	INT32 packetSize = sizeof(EntryMemberPacket);
	INT32 roomSize = sizeof(RoomMember);

	packet->Data.PacketType = PacketBuilder::HtonPacketType(packet->Data.PacketType);
	packet->Data.PacketLen = htonl(totalPacketSize);
	packet->Data.UserLocalId = htonl(userID);
	packet->Data.MemberCount = htonl(1);
	packet->Data.RoomID = htonl(roomID);
	packet->Data.RoomMemberLen = htonl(roomSize);

	RoomMember roomMember;
	if (entryRoomService->GetRoomMember(dbUserID, roomID, roomMember))
	{
		std::vector<char> buffer(totalPacketSize);
		memcpy_s(buffer.data(), packetSize, packet, packetSize);
		memcpy_s(buffer.data() + packetSize, roomSize, &roomMember, roomSize);
		socketWrapper->GroupBroadcast(roomID, buffer.data(), totalPacketSize, 0);
	}
	PacketBuilder::ReleasePacket<EntryMemberPacket>(&packet);
}
