#include "MainPCH.h"
#include "Header/Managers/DatabaseManager.h"

#include "Databases/DatabaseWrapper.h"
#include "Pool/DatabasePool.h"

SINGLETON_CLASS_CPP(DatabaseManager);

void DatabaseManager::Initialize(const char* ip, const char* user, const char* password, const char* db, INT32 port)
{
	DatabasePool = DatabasePool::CreateDatabasePool(ip, user, password, db, port);
	
}

INT32 DatabaseManager::GetUserID(UINT32 localUserID)
{
	std::unique_lock<std::shared_mutex> lock(LocalUserIdToUserIdMutex);
	if (LocalUserIdToUserId.find(localUserID) != LocalUserIdToUserId.cend())
	{
		return LocalUserIdToUserId[localUserID];
	}
	return -1;
}

void DatabaseManager::SetUserID(UINT32 localUserID, INT32 userID)
{
	std::unique_lock<std::shared_mutex> lock(LocalUserIdToUserIdMutex);
	if (LocalUserIdToUserId.find(localUserID) != LocalUserIdToUserId.cend())
	{
		LocalUserIdToUserId[localUserID] = userID;
	}
}

void DatabaseManager::RegisterId(UINT32 localUserID, INT32 userID)
{
	std::unique_lock<std::shared_mutex> lock(LocalUserIdToUserIdMutex);
	if (LocalUserIdToUserId.find(localUserID) == LocalUserIdToUserId.cend())
	{
		LocalUserIdToUserId.insert({ localUserID , userID });
	}
}

void DatabaseManager::RegisterId(UINT32 localUserID)
{
	std::unique_lock<std::shared_mutex> lock(LocalUserIdToUserIdMutex);
	if (LocalUserIdToUserId.find(localUserID) == LocalUserIdToUserId.cend())
	{
		LocalUserIdToUserId.insert({ localUserID , -1 });
	}
}

void DatabaseManager::UnRegisterUserID(UINT32 localUserID)
{
	std::unique_lock<std::shared_mutex> lock(LocalUserIdToUserIdMutex);
	if (LocalUserIdToUserId.find(localUserID) != LocalUserIdToUserId.cend())
	{
		LocalUserIdToUserId[localUserID] = 0;
	}
}

void DatabaseManager::UnRegisterLocalUserID(UINT32 localUserID)
{
	std::unique_lock<std::shared_mutex> lock(LocalUserIdToUserIdMutex);
	if (LocalUserIdToUserId.find(localUserID) != LocalUserIdToUserId.cend())
	{
		LocalUserIdToUserId.erase(localUserID);
	}
}
