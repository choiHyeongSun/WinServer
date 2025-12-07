#include "MainPCH.h"
#include "Header/Controllers/LoginController.h"
#include "Managers/RecvManager.h"
#include "Header/Repositories/UserRepository.h"
#include "Interfaces/ISocketWrapper.h"

CONTROLLER_CLASS_CPP(LoginController)

void LoginController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader)

	const PoolingObject<LoginPacket>* loginPacket = PacketBuilder::ConvertToPoolingObject<LoginPacket>(packet);

	PoolingObject<ResultMessage>* resultPack = PacketBuilder::PacketBuild<ResultMessage>();
	resultPack->Data.MessageType = PacketBuilder::HtonPacketType(resultPack->Data.MessageType);
	resultPack->Data.PrevMessageType = PacketBuilder::HtonPacketType(packetHeader->MessageType);
	resultPack->Data.MessageLen = htonl(sizeof(ResultMessage));
	if (loginService->Login(userID, loginPacket->Data.UserID, loginPacket->Data.Password))
	{
		resultPack->Data.ResultType = PacketBuilder::HtonPacketType(EMESSAGE_RESULT::SUCCESS);
		std::cout << loginPacket->Data.UserID << " 로그인에 성공 했습니다." << std::endl;
	}
	else
	{
		resultPack->Data.ResultType = PacketBuilder::HtonPacketType(EMESSAGE_RESULT::FAILED);
		std::cout << loginPacket->Data.UserID << " 로그인에 실패 했습니다." << std::endl;
	}

	const char* buffer = reinterpret_cast<const char*>(&resultPack->Data);
	size_t len = sizeof(ResultMessage);

	socketWrapper->PrivateMessage(userID, buffer, len, 0);
	PacketBuilder::ReleasePacket<ResultMessage>(&resultPack);
}
