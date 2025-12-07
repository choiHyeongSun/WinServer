#include "MainPCH.h"
#include "Header/Repositories/UserRepository.h"

#include "Header/Managers/DatabaseManager.h"
#include "Header/Databases/DatabaseWrapper.h"
#include "Pool/DatabasePool.h"
using namespace std::string_literals;

REPOSITORY_CLASS_CPP(UserRepository)


void UserRepository::Initialization()
{
	std::string userTableQuery =
		"CREATE TABLE IF NOT EXISTS users ("s + \
		"id INT AUTO_INCREMENT PRIMARY KEY, "s + \
		"Username VARCHAR(256) NOT NULL UNIQUE, "s + \
		"Password VARCHAR(256) NOT NULL , "s + \
		"Nickname VARCHAR(256) NOT NULL UNIQUE, "s + \
		"created_at TIMESTAMP DEFAULT NOW() "s + \
 		")";

	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	wrapper->CreateTable(userTableQuery.c_str());
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
}

void UserRepository::BindStmt()
{
	InsertQuery = "INSERT INTO users (Username, Password, Nickname) VALUES (? ,? ,?)";
	MYSQL_BIND mysql_bind;

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Username, UserEntity::MAX_CHAR_LENGTH, &Username_len);
	InsertBinds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Password, UserEntity::MAX_CHAR_LENGTH, &Password_len);
	InsertBinds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Nickname, UserEntity::MAX_CHAR_LENGTH, &Nickname_len);
	InsertBinds.push_back(mysql_bind);


	//--------------------------FindUserByUsername_stmt--------------------------
	FindUserByUsernameQuery = "SELECT* FROM users WHERE Username = ?";
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Username, UserEntity::MAX_CHAR_LENGTH, &Username_len);
	FindUserByUsernameBinds.push_back(mysql_bind);

	//--------------------------FindUserByNickname_stmt--------------------------
	FindUserByNicknameQuery = "SELECT* FROM users WHERE Nickname = ?";
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Nickname, UserEntity::MAX_CHAR_LENGTH, &Nickname_len);
	FindUserByNicknameBinds.push_back(mysql_bind);

	//--------------------------FindUserByd_stmt--------------------------
	FindUserByIdQuery = "SELECT * FROM users WHERE id = ?";
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_LONG, &UserID, sizeof(INT32));
	FindUserByIdBinds.push_back(mysql_bind);

	//--------------------------CanJoin_stmt--------------------------
	CanJoinQuery = "SELECT EXISTS (SELECT 1 FROM users WHERE username = ? OR nickname = ?)";
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Username, UserEntity::MAX_CHAR_LENGTH, &Username_len);
	CanJoinBinds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, Nickname, UserEntity::MAX_CHAR_LENGTH, &Nickname_len);
	CanJoinBinds.push_back(mysql_bind);
}

bool UserRepository::FindUserByUsername(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, UserEntity& outEntity)
{
	bool result = false;
	MYSQL_STMT* stmt = wrapper->StmtBind(FindUserByUsernameQuery.c_str(), FindUserByUsernameBinds);

	{
		std::unique_lock<std::shared_mutex> lock(UsernameMutex);
		strcpy_s(Username, username.c_str());
		Username_len = strlen(username.c_str());
		result = UniqueFind(stmt, outEntity);
	}
	
	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool UserRepository::FindUserByUserID(std::shared_ptr<DatabaseWrapper> wrapper, INT32 userID, UserEntity& outEntity)
{
	bool result = false;
	MYSQL_STMT* stmt = wrapper->StmtBind(FindUserByIdQuery.c_str(), FindUserByIdBinds);
	{
		std::unique_lock<std::shared_mutex> lock(UserIDMutex);
		UserID = userID;
		result = UniqueFind(stmt, outEntity);
	}

	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool UserRepository::FindUserByNickname(std::shared_ptr<DatabaseWrapper> wrapper, std::string nickname, UserEntity& outEntity)
{
	bool result = false;
	MYSQL_STMT* stmt = wrapper->StmtBind(FindUserByNicknameQuery.c_str(), FindUserByNicknameBinds);
	{
		std::unique_lock<std::shared_mutex> lock(NicknameMutex);
		strcpy_s(Nickname, nickname.c_str());
		Nickname_len = strlen(nickname.c_str());
		result = UniqueFind(stmt, outEntity);
	}

	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool UserRepository::CanJoin(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, std::string nickname)
{
	std::vector<MYSQL_BIND> binds;
	MYSQL_BIND mysql_bind;
	INT32 bindId;
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_LONG, &bindId, sizeof(INT32));
	binds.push_back(mysql_bind);

	MYSQL_STMT* stmt = wrapper->StmtBind(CanJoinQuery.c_str(), CanJoinBinds);
	{
		std::unique_lock<std::shared_mutex> nameLock(UsernameMutex);
		std::unique_lock<std::shared_mutex> nicknamelock(NicknameMutex);

		strcpy_s(Username, username.c_str());
		strcpy_s(Nickname, nickname.c_str());

		Username_len = strlen(username.c_str());
		Nickname_len = strlen(nickname.c_str());

		if (!DatabaseWrapper::ExecuteStmt(stmt, binds))
		{
			return false;
		}
	}
	if (DatabaseWrapper::FetchStmt(stmt) == false)
	{
		return false;
	}
	return bindId == 0;
}


bool UserRepository::InsertUser(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, std::string password, std::string nickname)
{

	MYSQL_STMT* stmt = wrapper->StmtBind(InsertQuery.c_str(), InsertBinds);
	{
		std::unique_lock<std::shared_mutex> nameLock(UsernameMutex);
		std::unique_lock<std::shared_mutex> passwordLock(PasswordMutex);
		std::unique_lock<std::shared_mutex> nicknamelock(NicknameMutex);

		strcpy_s(Username, username.c_str());
		strcpy_s(Password, password.c_str());
		strcpy_s(Nickname, nickname.c_str());


		Username_len = strlen(username.c_str());
		Password_len = strlen(password.c_str());
		Nickname_len = strlen(nickname.c_str());
		if (!DatabaseWrapper::ExecuteStmt(stmt))
		{
			return false;
		}
	}
	DatabaseWrapper::StmtClose(&stmt);
	return true;
}

bool UserRepository::UniqueFind(MYSQL_STMT* stmt, UserEntity& outUser) const
{
	UserEntity userEntity = { 0 };
	std::vector<MYSQL_BIND> binds;
	MYSQL_BIND mysql_bind;
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_LONG, &userEntity.userID, sizeof(userEntity.userID));
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, &userEntity.username, sizeof(UserEntity::MAX_CHAR_LENGTH));
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, &userEntity.password, sizeof(UserEntity::MAX_CHAR_LENGTH));
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, &userEntity.nickname, sizeof(UserEntity::MAX_CHAR_LENGTH));
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_DATETIME, &userEntity.created_at, sizeof(MYSQL_TIME));
	binds.push_back(mysql_bind);

	DatabaseWrapper::ExecuteStmt(stmt, binds);
	bool state = DatabaseWrapper::FetchStmt(stmt);
	if (state)
	{
		outUser = userEntity;
	}

	DatabaseWrapper::ReleaseStmtResult(stmt);
	return state;
}