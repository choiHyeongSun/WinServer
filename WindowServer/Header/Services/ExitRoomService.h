#pragma once
#include "Repositories/ChattingRoomRepository.h"
#include "Repositories/RoomMemberRepository.h"

class ExitRoomService
{
	SERVICE_CLASS_H(ExitRoomService);
	INJECTION(ChattingRoomRepository, chattingRoomRepository);
	INJECTION(RoomMemberRepository, roomMemberRepository);


public:
	bool ExitRoom(INT32 userID ,UINT32 roomID);
	bool GetRoomMember(INT32 userID, INT32 roomID, RoomMember& outRoomMember);
};
