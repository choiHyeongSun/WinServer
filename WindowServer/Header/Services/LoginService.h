#pragma once
#include "SendMsgTypes.h"
#include "Repositories/ChattingRoomRepository.h"
#include "Repositories/UserRepository.h"


class LoginService
{
private:
	SERVICE_CLASS_H(LoginService);
	INJECTION(UserRepository, userRepository);
	INJECTION(ChattingRoomRepository, chattingRoomRepository);

public:
	bool Login(UINT32 localUserId, std::string username, std::string password);
	bool FindAllChatRoom(std::vector<RoomInfoPacket>& outRooms);
};
