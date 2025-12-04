#include "MainPCH.h"
#include "Header/Controllers/TestController.h"

CONTROLLER_CLASS_CPP(TestController)

void TestController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader)
	const PoolingObject<TestPacket>* testPacket = reinterpret_cast<const PoolingObject<TestPacket>*>(packet);
	std::cout << "Received Test Packet from User ID: " << userID << std::endl;
}
