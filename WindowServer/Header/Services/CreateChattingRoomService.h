#pragma once
#include "Repositories/ChattingRoomRepository.h"

class CreateChattingRoomService
{
	SERVICE_CLASS_H(CreateChattingRoomService)
	INJECTION(ChattingRoomRepository, chattingRoomRepository);

public:
	bool CreateChattingRoom(std::string name, std::string password, INT32 & outID, std::string & outMsg);
};
