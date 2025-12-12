#include "MainPCH.h"
#include "Header/Controllers/JoinController.h"

#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(JoinController)
void JoinController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
                                   ISocketWrapper* socketWrapper)
{
    PACKET_CHECKER(packetHeader)
    const JoinPacket* message = reinterpret_cast<const JoinPacket*>(packet);

    std::string msg;
    if (joinService->JoinUser(message->UserID, message->Password, message->Nickname, msg))
    {
        SendResult(socketWrapper, EMESSAGE_RESULT::SUCCESS, packetHeader, msg);
    }
    else
    {
        SendResult(socketWrapper, EMESSAGE_RESULT::FAILED, packetHeader, msg);
    }
};
