#include "MainPCH.h"
#include "Header/Controllers/LogoutController.h"

#include "Utility.h"
#include "Interfaces/ISocketWrapper.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(LogoutController)

void LogoutController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
	ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader)
	PoolingObject<ResultPacket>* result = PacketBuilder::PacketBuild<ResultPacket>();
	std::string msg = "로그아웃에 성공 했습니다.";
	msg = Utility::ToUTF8(msg);
	logoutService->LogoutProcess(userID);

	INT32 len = sizeof(ResultPacket);
	result->Data.PacketType = PacketBuilder::HtonPacketType(result->Data.PacketType);
	result->Data.PrevMessageType = PacketBuilder::HtonPacketType(packetHeader->PacketType);
	result->Data.ResultType = PacketBuilder::HtonPacketType(EMESSAGE_RESULT::SUCCESS);
	result->Data.UserLocalId = htonl(result->Data.UserLocalId);
	result->Data.PacketLen = htonl(len);

	memcpy_s(result->Data.ResultMsg, 256, msg.c_str(), msg.length());

	const char* buffer = reinterpret_cast<const char*>(&result->Data);
	socketWrapper->PrivateMessage(userID, buffer, len, 0);
}

