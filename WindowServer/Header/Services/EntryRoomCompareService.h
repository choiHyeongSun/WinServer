#pragma once
#include "Repositories/ChattingRoomRepository.h"

class EntryRoomCompareService
{
private:
	SERVICE_CLASS_H(EntryRoomCompareService);
	INJECTION(ChattingRoomRepository, chattingRoomRepository);

public:
	bool CompareRoom(UINT32 roomId);
};
