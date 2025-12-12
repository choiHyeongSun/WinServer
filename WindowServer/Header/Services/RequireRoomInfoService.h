#pragma once
#include "SendMsgTypes.h"
#include "Repositories/MessageRepository.h"
#include "Repositories/RoomMemberRepository.h"

class RequireRoomInfoService
{
	SERVICE_CLASS_H(RequireRoomInfoService);
	INJECTION(RoomMemberRepository, roomMemberRepository);
	INJECTION(MessageRepository, messageRepository);

public:
	bool FindAllRoomMember(INT32 userID, INT32 roomID, std::vector<RoomMember> & roomMember);
	bool FindAllMessages(INT32 roomID, std::vector<std::pair<MessageHeader, std::string>>& outMessages);
};
