#include "MainPCH.h"
#include "Header/Controllers/ExitClientController.h"

#include "Managers/RecvManager.h"

CONTROLLER_CLASS_CPP(ExitClientController)

void ExitClientController::OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet,
	INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	exitClientService->ExitClientProcess(userID);
};
