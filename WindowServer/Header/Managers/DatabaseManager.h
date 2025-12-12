#pragma once
#include <shared_mutex>

class DatabasePool;
class DatabaseWrapper;

class DatabaseManager
{
	SINGLETON_CLASS_H(DatabaseManager)


public:
	FORCEINLINE static std::shared_ptr<DatabasePool> GetDatabasePool() { return instance->DatabasePool; }

public:
	void Initialize(const char* ip, const char* user, const char* password, const char* db, INT32 port);
	INT32 GetUserID(UINT32 localUserID);
	void SetUserID(UINT32 localUserID, INT32 userID);
	void RegisterId(UINT32 localUserID, INT32 userID);
	void RegisterId(UINT32 localUserID);
	void UnRegisterUserID(UINT32 localUserID);
	void UnRegisterLocalUserID(UINT32 localUserID);



private:
	std::shared_ptr<DatabasePool> DatabasePool;
	std::unordered_map<UINT32, INT32> LocalUserIdToUserId;

	std::shared_mutex LocalUserIdToUserIdMutex;
};
