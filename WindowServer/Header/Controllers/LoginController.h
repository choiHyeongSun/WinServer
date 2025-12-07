#pragma once
#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include "Repositories/UserRepository.h"
#include "Services/LoginService.h"

class UserRepository;

class LoginController : public IRecvMessage
{
private:
	CONTROLLER_CLASS_H(LoginController, EMESSAGE_TYPE::LOGIN_PACKET);
	INJECTION(LoginService, loginService);

public:
	void OnRecvMessage(UINT32 userID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper) override;
};
