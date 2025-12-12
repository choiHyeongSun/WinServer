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


bool UserRepository::FindUserByUsername(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, UserEntity& outEntity)
{
	char username_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	UINT32 username_len = 0;

	std::string query = "SELECT* FROM users WHERE Username = ?";

	MYSQL_BIND mysql_bind = { 0 };
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, username_buffer, UserEntity::MAX_CHAR_LENGTH, &username_len);

	bool result = false;
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &mysql_bind);
	{
		memcpy_s(username_buffer, UserEntity::MAX_CHAR_LENGTH, username.c_str(), username.size());
		username_len = strlen(username_buffer);
		result = UniqueFind(stmt, outEntity);
	}
	
	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool UserRepository::FindUserByUserID(std::shared_ptr<DatabaseWrapper> wrapper, INT32 userID, UserEntity& outEntity)
{
	bool result = false;

	INT32 userId_buffer = 0;
	std::string query = "SELECT * FROM users WHERE id = ?";

	MYSQL_BIND mysql_bind;
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_LONG, &userId_buffer, sizeof(INT32));

	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &mysql_bind);
	{
		userId_buffer = userID;
		result = UniqueFind(stmt, outEntity);
	}

	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool UserRepository::FindUserByNickname(std::shared_ptr<DatabaseWrapper> wrapper, std::string nickname, UserEntity& outEntity)
{

	char nickname_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	UINT32 nickname_len = 0;

	std::string query = "SELECT* FROM users WHERE Nickname = ?";

	MYSQL_BIND mysql_bind;
	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, nickname_buffer, UserEntity::MAX_CHAR_LENGTH, &nickname_len);

	bool result = false;
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &mysql_bind);
	{
		memcpy_s(nickname_buffer, UserEntity::MAX_CHAR_LENGTH, nickname.c_str(), nickname.size());
		nickname_len = strlen(nickname.c_str());
		result = UniqueFind(stmt, outEntity);
	}

	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool UserRepository::IsExistUsername(std::shared_ptr<DatabaseWrapper> wrapper, std::string username)
{
	char username_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	UINT32 username_len = 0;

	MYSQL_BIND in_mysql_bind;
	std::string query = "SELECT EXISTS (SELECT 1 FROM users WHERE username = ?)";
	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_STRING, username_buffer, UserEntity::MAX_CHAR_LENGTH, &username_len);


	MYSQL_BIND out_mysql_bind;
	INT32 bindId;
	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &bindId, sizeof(INT32));

	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);
	{
		memcpy_s(username_buffer, UserEntity::MAX_CHAR_LENGTH, username.c_str(), username.size());
		username_len = strlen(username_buffer);

		if (!DatabaseWrapper::ExecuteStmt(stmt, &out_mysql_bind))
		{
			return false;
		}
	}

	if (DatabaseWrapper::FetchStmt(stmt) == false)
	{
		return false;
	}
	DatabaseWrapper::StmtClose(&stmt);
	return bindId != 0;
}

bool UserRepository::IsExistUserNickname(std::shared_ptr<DatabaseWrapper> wrapper, std::string nickname)
{
	char nickname_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	UINT32 nickname_len = 0;

	std::string query = "SELECT EXISTS (SELECT 1 FROM users WHERE nickname = ?)";
	MYSQL_BIND in_mysql_bind;
	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_STRING, nickname_buffer, UserEntity::MAX_CHAR_LENGTH, &nickname_len);

	std::vector<MYSQL_BIND> binds;
	MYSQL_BIND out_mysql_bind;
	INT32 bindId;
	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &bindId, sizeof(INT32));

	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);
	{
		strcpy_s(nickname_buffer, nickname.c_str());
		nickname_len = strlen(nickname_buffer);

		if (!DatabaseWrapper::ExecuteStmt(stmt, &out_mysql_bind))
		{
			return false;
		}
	}
	if (DatabaseWrapper::FetchStmt(stmt) == false)
	{
		return false;
	}
	DatabaseWrapper::StmtClose(&stmt);
	return bindId != 0;
}


bool UserRepository::InsertUser(std::shared_ptr<DatabaseWrapper> wrapper, std::string username, std::string password, std::string nickname)
{
	std::vector<MYSQL_BIND> InsertBinds(3);
	char username_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	char password_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };
	char nickname_buffer[UserEntity::MAX_CHAR_LENGTH] = { 0 };

	UINT32 username_len = 0;
	UINT32 password_len = 0;
	UINT32 nickname_len = 0;

	std::string query = "INSERT INTO users (Username, Password, Nickname) VALUES (? ,? ,?)";

	DatabaseWrapper::SetBind(InsertBinds[0], MYSQL_TYPE_STRING, username_buffer, UserEntity::MAX_CHAR_LENGTH, &username_len);
	DatabaseWrapper::SetBind(InsertBinds[1], MYSQL_TYPE_STRING, password_buffer, UserEntity::MAX_CHAR_LENGTH, &password_len);
	DatabaseWrapper::SetBind(InsertBinds[2], MYSQL_TYPE_STRING, nickname_buffer, UserEntity::MAX_CHAR_LENGTH, &nickname_len);

	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), InsertBinds.data());
	{
		memcpy_s(username_buffer, UserEntity::MAX_CHAR_LENGTH ,username.c_str(), username.size());
		memcpy_s(password_buffer, UserEntity::MAX_CHAR_LENGTH ,password.c_str(), password.size());
		memcpy_s(nickname_buffer, UserEntity::MAX_CHAR_LENGTH ,nickname.c_str(), username.size());
	
		username_len = strlen(username_buffer);
		password_len = strlen(password_buffer);
		nickname_len = strlen(nickname_buffer);
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

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, &userEntity.username, UserEntity::MAX_CHAR_LENGTH);
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, &userEntity.password, UserEntity::MAX_CHAR_LENGTH);
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_STRING, &userEntity.nickname, UserEntity::MAX_CHAR_LENGTH);
	binds.push_back(mysql_bind);

	DatabaseWrapper::SetBind(mysql_bind, MYSQL_TYPE_DATETIME, &userEntity.created_at, sizeof(MYSQL_TIME));
	binds.push_back(mysql_bind);

	if (!DatabaseWrapper::ExecuteStmt(stmt, binds.data()))
	{
		return false;
	}
	bool state = DatabaseWrapper::FetchStmt(stmt);
	if (state)
	{
		outUser = userEntity;
	}

	DatabaseWrapper::ReleaseStmtResult(stmt);
	return state;
}