#include "MainPCH.h"
#include "Header/Services/SendMessageService.h"

#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(SendMessageService)

bool SendMessageService::InsertMessage(INT32 roomID, INT32 userID, char* msg, INT32 len)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	bool result = messageRepository->InsertMessage(wrapper.get(), roomID, userID, msg, len);
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return result;
};
