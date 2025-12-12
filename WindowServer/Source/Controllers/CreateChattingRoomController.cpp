#include "MainPCH.h"
#include "Header/Controllers/CreateChattingRoomController.h"

#include <string>

#include "Interfaces/ISocketWrapper.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(CreateChattingRoomController)

void CreateChattingRoomController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet,
	INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader);
	const CreateRoomPacket* roomPacket = reinterpret_cast<const CreateRoomPacket*>(packet);

	std::string msg;
	INT32 insert_id = 0;
	if (createChattingRoomService->CreateChattingRoom(roomPacket->RoomName, roomPacket->Password, insert_id, msg))
	{
		msg = std::to_string(insert_id);
		SendResult(socketWrapper, EMESSAGE_RESULT::SUCCESS, packetHeader, msg);

		int packetSize = sizeof(SendRoomPacket);
		int roomInfoSize = sizeof(RoomInfoPacket);
		int totalSize = packetSize + roomInfoSize;

		PoolingObject<SendRoomPacket>* pooling = PacketBuilder::PacketBuild<SendRoomPacket>();
		pooling->Data.Count = htonl(1);
		pooling->Data.ArrayBufferLen = htonl(sizeof(RoomInfoPacket));
		pooling->Data.PacketLen = htonl(totalSize);
		pooling->Data.PacketType = PacketBuilder::HtonPacketType(pooling->Data.PacketType);
		pooling->Data.UserLocalId = htonl(userID);

		RoomInfoPacket roomInfo;
		roomInfo.roomID = insert_id;
		memcpy_s(roomInfo.roomName, 256, roomPacket->RoomName, 256);

		std::vector<char> buffers(totalSize);
		memcpy_s(buffers.data(), totalSize, &pooling->Data, packetSize);
		memcpy_s(buffers.data() + packetSize, totalSize, &roomInfo, roomInfoSize);


		socketWrapper->Broadcast(buffers.data(), totalSize, 0);
		PacketBuilder::ReleasePacket(&pooling);
	}
	else
	{
		SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
	}
};

