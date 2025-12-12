#include "MainPCH.h"
#include "Header/Controllers/ExitRoomController.h"

#include <string>

#include "Utility.h"
#include "Interfaces/ISocketWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(ExitRoomController);

void ExitRoomController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet,
	INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader);
	const ExitRoomPacket* exitRoomPacket = reinterpret_cast<const ExitRoomPacket*>(packet);
	INT32 roomID = htonl(exitRoomPacket->RoomId);

	std::string msg;
	INT32 dbUserID = DatabaseManager::getInstance()->GetUserID(userID);
	SendExit(userID, roomID, socketWrapper);
	socketWrapper->ExitGroup(roomID, userID);
	if (exitRoomService->ExitRoom(dbUserID, roomID))
	{
		msg = Utility::ToUTF8(std::to_string(roomID));
		PoolingObject<ResultPacket>* result = PacketBuilder::PacketBuild<ResultPacket>();
		GetResult(EMESSAGE_RESULT::SUCCESS, packetHeader, msg, result);
		const char* buffer = reinterpret_cast<const char*>(&result->Data);
		const size_t len = sizeof(ResultPacket);

		
		socketWrapper->Broadcast(buffer, len, 0);
		PacketBuilder::ReleasePacket<ResultPacket>(&result);
	}
	else
	{
		msg = Utility::ToUTF8("방을 제거 하지 못했습니다.");
		SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
	}
}

void ExitRoomController::SendExit(UINT32 userID, INT32 roomID, ISocketWrapper* socketWrapper)
{
	PoolingObject<ExitMemberPacket>* packet = PacketBuilder::PacketBuild<ExitMemberPacket>();
	INT32 dbUserID = DatabaseManager::getInstance()->GetUserID(userID);

	INT32 totalPacketSize = sizeof(ExitMemberPacket) + sizeof(RoomMember);
	INT32 packetSize = sizeof(ExitMemberPacket);
	INT32 roomSize = sizeof(RoomMember);

	packet->Data.PacketType = PacketBuilder::HtonPacketType(packet->Data.PacketType);
	packet->Data.PacketLen = htonl(totalPacketSize);
	packet->Data.UserLocalId = htonl(userID);
	packet->Data.MemberCount = htonl(1);
	packet->Data.RoomID = htonl(roomID);
	packet->Data.RoomMemberLen = htonl(roomSize);

	RoomMember roomMember;
	if (exitRoomService->GetRoomMember(dbUserID, roomID, roomMember))
	{
		std::vector<char> buffer(totalPacketSize);
		memcpy_s(buffer.data(), totalPacketSize, packet, packetSize);
		memcpy_s(buffer.data() + packetSize, totalPacketSize, &roomMember, roomSize);
		socketWrapper->GroupBroadcast(roomID, buffer.data(), totalPacketSize, 0);
	}
	PacketBuilder::ReleasePacket<ExitMemberPacket>(&packet);
}
