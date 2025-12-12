#include "MainPCH.h"
#include "Controllers/SendMessageController.h"

#include "Interfaces/ISocketWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(SendMessageController)

void SendMessageController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet,
	INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	PACKET_CHECKER(packetHeader);

	const SendMessagePacket* sendMessagePacket = reinterpret_cast<const SendMessagePacket*>(packet);
	INT32 roomID = ntohl(sendMessagePacket->RoomID);
	INT32 packetLen = ntohl(sendMessagePacket->PacketLen);
	INT32 count = ntohl(sendMessagePacket->MessageCount);


	INT32 currentLen = sizeof(SendMessagePacket);
	for (int i = 0; i < count; i++)
	{
		const MessageHeader * header = reinterpret_cast<const MessageHeader*>(packet + currentLen);
		currentLen += sizeof(MessageHeader);
		INT32 msgLen = htonl(header->MsgLen);
		INT32 userID = ntohl(header->SendUserID);
		INT32 dbUserID = DatabaseManager::getInstance()->GetUserID(userID);
		std::vector<char> buffer(msgLen);

		memcpy_s(buffer.data(), msgLen, packet + currentLen, msgLen);
		sendMessageService->InsertMessage(roomID, dbUserID, buffer.data(), msgLen);

		currentLen += msgLen;
	}

	socketWrapper->GroupBroadcast(roomID, packet, packetLen, 0);
};
