#include "MainPCH.h"
#include "Header/Pool/DatabasePool.h"

#include "Databases/DatabaseWrapper.h"


std::shared_ptr<DatabasePool> DatabasePool::CreateDatabasePool(const char* ip, const char* user, const char* password, const char* db, INT32 port)
{
	std::shared_ptr<DatabasePool> newPool = std::shared_ptr<DatabasePool>(new DatabasePool());
	newPool->Initialization(ip, user, password, db, port);
	return newPool;
}

void DatabasePool::Initialization(const char* ip, const char* user, const char* password, const char* db, INT32 port)
{
	for (int i = 0; i < MAX_POOL_SIZE; i++)
	{
		std::shared_ptr<DatabaseWrapper> wrapper = DatabaseWrapper::CreateDatabase();
		wrapper->ConnectDB(ip, user, password, db, port);
		Databases.push(wrapper);
	}

	DBSemaphore = CreateSemaphore(nullptr, MAX_POOL_SIZE, MAX_POOL_SIZE, nullptr);
	std::cout << "Created Database Pool" << std::endl;
}

std::shared_ptr<DatabaseWrapper> DatabasePool::PopWrapper()
{
	WaitForSingleObject(DBSemaphore, INFINITE);
	std::unique_lock<std::shared_mutex> lock(DatabasesMutex);
	std::shared_ptr<DatabaseWrapper> result = Databases.front(); Databases.pop();
	return result;
}

void DatabasePool::PushWrapper(std::shared_ptr<DatabaseWrapper>& database)
{
	std::unique_lock<std::shared_mutex> lock(DatabasesMutex);
	Databases.push(database);
	database = nullptr;
	if (!ReleaseSemaphore(DBSemaphore, 1, nullptr))
	{
		std::cout << "DBSemaphore Release Error" << std::endl;
		return;
	}
}


