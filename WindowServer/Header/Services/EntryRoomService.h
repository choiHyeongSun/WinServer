#pragma once
#include "Repositories/ChattingRoomRepository.h"
#include "Repositories/RoomMemberRepository.h"

class EntryRoomService
{
	SERVICE_CLASS_H(EntryRoomService);
	INJECTION(ChattingRoomRepository, chattingRoomRepository);
	INJECTION(RoomMemberRepository, roomMemberRepository);

public:
	bool PasswordCompare(INT32 userID,INT32 roomID, std::string password);
	bool GetRoomMember(INT32 userID, INT32 roomID, RoomMember& outRoomMember);

};
