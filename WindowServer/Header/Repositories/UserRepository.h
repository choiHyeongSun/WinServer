#pragma once
#include <mysql.h>
#include <shared_mutex>

#include "Entities/UserEntity.h"
#include "Interfaces/IRepository.h"

class DatabaseWrapper;

class UserRepository : public IRepository
{
	REPOSITORY_CLASS_H(UserRepository)

protected:
	virtual void Initialization() override;

public:
	bool FindUserByUsername(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, UserEntity& outEntity);
	bool FindUserByUserID(std::shared_ptr<DatabaseWrapper> wrapper, INT32 userID, UserEntity& outEntity);
	bool FindUserByNickname(std::shared_ptr<DatabaseWrapper> wrapper, std::string nickname, UserEntity& outEntity);
	bool IsExistUsername(std::shared_ptr<DatabaseWrapper> wrapper, std::string username);
	bool IsExistUserNickname(std::shared_ptr<DatabaseWrapper> wrapper, std::string nickname);
	bool InsertUser(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, std::string password, std::string nickname);
private:
	bool UniqueFind(MYSQL_STMT* stmt, UserEntity & outUser) const;

};
