#include "MainPCH.h"
#include "Header/Controllers/JoinController.h"

#include "Interfaces/ISocketWrapper.h"
#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(JoinController)

void JoinController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
                                   ISocketWrapper* socketWrapper)
{
    PACKET_CHECKER(packetHeader)
    const PoolingObject<JoinPacket>* poolingPack = reinterpret_cast<const PoolingObject<JoinPacket>*>(packet);
    const JoinPacket *message = &poolingPack->Data;

    PoolingObject<ResultMessage>* result = PacketBuilder::PacketBuild<ResultMessage>();

    result->Data.MessageType = PacketBuilder::HtonPacketType(result->Data.MessageType);
    result->Data.PrevMessageType = PacketBuilder::HtonPacketType(packetHeader->MessageType);
    result->Data.MessageLen = htonl(sizeof(ResultMessage));


    if (joinService->JoinUser(message->UserID, message->Password, message->Nickname))
    {
        result->Data.ResultType = PacketBuilder::HtonPacketType(EMESSAGE_RESULT::SUCCESS);
    }
    else
    {
        result->Data.ResultType = PacketBuilder::HtonPacketType(EMESSAGE_RESULT::FAILED);
    }

	const char* buffer = reinterpret_cast<const char*>(&result->Data);
    const size_t len = sizeof(ResultMessage);

	socketWrapper->PrivateMessage(userID, buffer, len, 0);
    PacketBuilder::ReleasePacket(EMESSAGE_TYPE::RESULT_MESSAGE, reinterpret_cast<void**>(&result));
};
