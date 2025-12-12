#include "MainPCH.h"
#include "Header/Services/EntryRoomCompareService.h"

#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(EntryRoomCompareService)

bool EntryRoomCompareService::CompareRoom(UINT32 roomId)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	bool isCompare = chattingRoomRepository->ExistPasswordRoom(wrapper.get(), roomId);
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return isCompare;
};


