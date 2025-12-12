#include "MainPCH.h"

#include "Header/Controllers/RequireRoomInfoController.h"

#include "Interfaces/ISocketWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(RequireRoomInfoController)

void RequireRoomInfoController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet,
	INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader);
	const RequireRoomInfoPacket * requireRoomInfoPacket = reinterpret_cast<const RequireRoomInfoPacket*>(packet);
	const INT32 roomId = ntohl(requireRoomInfoPacket->RoomId);
	const INT32 dbUserID = DatabaseManager::getInstance()->GetUserID(userID);

	std::vector<RoomMember> roomMembers;
	if (requireRoomInfoService->FindAllRoomMember(dbUserID, roomId, roomMembers))
	{
		PoolingObject<EntryMemberPacket>* membersPacket = PacketBuilder::PacketBuild<EntryMemberPacket>();
		INT32 totalPacketSize = sizeof(EntryMemberPacket) + sizeof(RoomMember) * roomMembers.size();
		INT32 packetSize = sizeof(EntryMemberPacket);
		INT32 roomSize = sizeof(RoomMember);

		membersPacket->Data.PacketType = PacketBuilder::HtonPacketType(membersPacket->Data.PacketType);
		membersPacket->Data.PacketLen = htonl(totalPacketSize);
		membersPacket->Data.UserLocalId = htonl(userID);
		membersPacket->Data.MemberCount = htonl(roomMembers.size());
		membersPacket->Data.RoomID = htonl(roomId);
		membersPacket->Data.RoomMemberLen = htonl(roomSize);

		std::vector<char> buffer(totalPacketSize);
		INT32 roomMemberSize = roomSize * roomMembers.size();
		memcpy_s(buffer.data(), packetSize, membersPacket, packetSize);
		memcpy_s(buffer.data() + packetSize, roomMemberSize, roomMembers.data(), roomMemberSize);
		socketWrapper->PrivateMessage(userID, buffer.data(), totalPacketSize, 0);

		PacketBuilder::ReleasePacket<EntryMemberPacket>(&membersPacket);
	}

	std::vector<std::pair<MessageHeader, std::string>> roomMessages;
	if (requireRoomInfoService->FindAllMessages(roomId, roomMessages))
	{
		if (roomMessages.empty()) return;

		INT32 packetSize = sizeof(SendMessagePacket);
		INT32 headerSize = sizeof(MessageHeader);
		INT32 totalHeaderSize = roomMessages.size() * headerSize;

		PoolingObject<SendMessagePacket>* sendMsgPacket = PacketBuilder::PacketBuild<SendMessagePacket>();
		sendMsgPacket->Data.PacketType = PacketBuilder::HtonPacketType(sendMsgPacket->Data.PacketType);
		sendMsgPacket->Data.UserLocalId = htonl(userID);
		sendMsgPacket->Data.RoomID = htonl(roomId);
		sendMsgPacket->Data.MessageCount = htonl(roomMessages.size());

		INT32 totalPacketSize = packetSize + totalHeaderSize;
		for (int i = 0; i < roomMessages.size(); i ++)
		{
			totalPacketSize += roomMessages[i].second.size();
			roomMessages[i].first.MsgLen = htonl(roomMessages[i].second.size());
			roomMessages[i].first.SendUserID = htonl(roomMessages[i].first.SendUserID);
		}
		sendMsgPacket->Data.PacketLen = htonl(totalPacketSize);

		std::vector<char> buffer(totalPacketSize);
		memcpy_s(buffer.data(), packetSize, sendMsgPacket, packetSize);
		totalPacketSize = packetSize;
		for (int i = 0; i < roomMessages.size(); i++)
		{
			INT32 len = roomMessages[i].second.size();
			memcpy_s(buffer.data() + totalPacketSize, headerSize, &roomMessages[i].first, headerSize);
			totalPacketSize += headerSize;
			memcpy_s(buffer.data() + totalPacketSize, len, roomMessages[i].second.data(), len);
			totalPacketSize += len;
		}
		socketWrapper->PrivateMessage(userID, buffer.data(), totalPacketSize, 0);
		PacketBuilder::ReleasePacket<SendMessagePacket>(&sendMsgPacket);
	}

	


};
