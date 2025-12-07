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
	virtual void BindStmt() override;

public:
	bool FindUserByUsername(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, UserEntity& outEntity);
	bool FindUserByUserID(std::shared_ptr<DatabaseWrapper> wrapper, INT32 userID, UserEntity& outEntity);
	bool FindUserByNickname(std::shared_ptr<DatabaseWrapper> wrapper, std::string nickname, UserEntity& outEntity);
	bool CanJoin(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, std::string nickname);
	bool InsertUser(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, std::string password, std::string nickname);

private :
	bool UniqueFind(MYSQL_STMT* stmt, UserEntity & outUser) const;

private:
	std::string InsertQuery;
	std::vector<MYSQL_BIND> InsertBinds;

	std::string FindUserByUsernameQuery;
	std::vector<MYSQL_BIND> FindUserByUsernameBinds;

	std::string FindUserByNicknameQuery;
	std::vector<MYSQL_BIND> FindUserByNicknameBinds;

	std::string FindUserByIdQuery;
	std::vector<MYSQL_BIND> FindUserByIdBinds;

	std::string CanJoinQuery;
	std::vector<MYSQL_BIND> CanJoinBinds;

private:
	INT32 UserID = 0;
	char Username[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	char Password[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	char Nickname[UserEntity::MAX_CHAR_LENGTH] = { 0 };


	UINT32 Username_len = 0;
	UINT32 Password_len = 0;
	UINT32 Nickname_len = 0;

	std::shared_mutex UsernameMutex;
	std::shared_mutex PasswordMutex;
	std::shared_mutex NicknameMutex;
	std::shared_mutex UserIDMutex;
};
