#include "MainPCH.h"
#include "Header/Controllers/LoginController.h"

#include "Utility.h"
#include "Managers/RecvManager.h"
#include "Header/Repositories/UserRepository.h"
#include "Interfaces/ISocketWrapper.h"

CONTROLLER_CLASS_CPP(LoginController)

void LoginController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader)

	const PoolingObject<LoginPacket>* loginPacket = PacketBuilder::ConvertToPoolingObject<LoginPacket>(packet);

	std::string msg;
	if (loginService->Login(userID, loginPacket->Data.UserID, loginPacket->Data.Password))
	{
		msg = "로그인에 성공 했습니다.";
		SendResult(socketWrapper, EMESSAGE_RESULT::SUCCESS, packetHeader, msg);

		std::vector<RoomInfoPacket> roomInfoPackets;
		if (loginService->FindAllChatRoom(roomInfoPackets))
		{
			INT32 packlen = sizeof(SendRoomPacket);
			INT32 roomInfosLen = sizeof(RoomInfoPacket) * roomInfoPackets.size();
			INT32 totalLen = packlen + roomInfosLen;
			PoolingObject<SendRoomPacket>* sendRoomPacket = PacketBuilder::PacketBuild<SendRoomPacket>();

			sendRoomPacket->Data.PacketType = PacketBuilder::HtonPacketType(sendRoomPacket->Data.PacketType);
			sendRoomPacket->Data.PacketLen = htonl(totalLen);
			sendRoomPacket->Data.UserLocalId = htonl(userID);

			sendRoomPacket->Data.Count = htonl(roomInfoPackets.size());
			sendRoomPacket->Data.ArrayBufferLen = htonl(roomInfosLen);

			std::vector<char> buffer(totalLen);
			memcpy_s(buffer.data(), totalLen, &sendRoomPacket->Data, packlen);
			memcpy_s(buffer.data() + packlen, totalLen, roomInfoPackets.data(), roomInfosLen);

			socketWrapper->PrivateMessage(userID, buffer.data(), totalLen, 0);
			PacketBuilder::ReleasePacket<SendRoomPacket>(&sendRoomPacket);
		}
	}
	else
	{
		msg = "로그인에 실패 했습니다.";
		SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
	}
}
