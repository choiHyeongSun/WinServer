#include "MainPCH.h"
#include "Header/Controllers/LoginController.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(LoginController)

void LoginController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader)

	const PoolingObject<LoginPacket>* loginPacket = reinterpret_cast<const PoolingObject<LoginPacket>*>(packet);

	std::wcout << loginPacket->Data.UserID << std::endl;
	std::wcout << loginPacket->Data.Password << std::endl;
}
