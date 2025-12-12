#pragma once
#include "Controller.h"
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Header/Repositories/UserRepository.h"
#include "Services/JoinService.h"

class JoinController : public Controller ,public IRecvMessage
{
private:
	CONTROLLER_CLASS_H(JoinController, EMESSAGE_TYPE::JOIN_PACKET);
	INJECTION(JoinService, joinService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen,
		ISocketWrapper* socketWrapper) override;
};
