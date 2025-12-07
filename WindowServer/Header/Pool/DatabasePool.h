#pragma once
#include <shared_mutex>

class DatabaseWrapper;


class DatabasePool
{
private:
	static constexpr int MAX_POOL_SIZE = 40;

public:
	static std::shared_ptr<DatabasePool> CreateDatabasePool(const char* ip, const char* user, const char* password, const char* db, INT32 port);

private:
	DatabasePool() {}
	  

public:
	std::shared_ptr<DatabaseWrapper> PopWrapper();
	void PushWrapper(std::shared_ptr<DatabaseWrapper>& database);

private:
	void Initialization(const char* ip, const char* user, const char* password, const char* db, INT32 port);
;
private:
	std::queue<std::shared_ptr<DatabaseWrapper>> Databases;

	HANDLE DBSemaphore;
	std::shared_mutex DatabasesMutex;
};


