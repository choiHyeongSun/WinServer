#include "MainPCH.h"
#include "Header/Services/JoinService.h"

#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(JoinService)

bool JoinService::JoinUser(std::string username, std::string password, std::string nickname)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();

	if (userRepository->CanJoin(wrapper, username, nickname))
	{
		if (userRepository->InsertUser(wrapper, username, password, nickname))
		{
			DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
			return true;
		}
	}
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return false;
}