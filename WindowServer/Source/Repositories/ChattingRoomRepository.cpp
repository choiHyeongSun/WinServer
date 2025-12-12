#include "MainPCH.h"
#include "Header/Repositories/ChattingRoomRepository.h"

#include "SendMsgTypes.h"
#include "Databases/DatabaseWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

using namespace std::string_literals;

REPOSITORY_CLASS_CPP(ChattingRoomRepository)

void ChattingRoomRepository::Initialization()
{
	std::string query =
		"CREATE TABLE IF NOT EXISTS chattingroom( "s + \
		"id INT AUTO_INCREMENT PRIMARY KEY, "s + \
		"ChattingRoomName VARCHAR(256) NOT NULL, "s + \
		"Password VARCHAR(256), "s + \
		"Create_at TIMESTAMP DEFAULT NOW() "s
		")";

	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	wrapper->CreateTable(query.c_str());
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
}

bool ChattingRoomRepository::InsertChattingRoom(DatabaseWrapper* wrapper, std::string name, std::string password, INT32& outid)
{
	std::vector<MYSQL_BIND> binds(2);

	char roomName[ChattingRoomEntity::MAX_NAME_CHAR_LENGTH]{ 0 };
	char roomPassword[ChattingRoomEntity::MAX_NAME_CHAR_LENGTH]{ 0 };

	UINT32 roomNameLen = 0;
	UINT32 roomPasswordLen = 0;

	bool IsPasswordNull = false;
	if (password.empty())
	{
		IsPasswordNull = true;
	}

	std::string query = "INSERT INTO chattingroom (ChattingRoomName, Password) VALUES (?, ?)";
	DatabaseWrapper::SetBind(binds[0], MYSQL_TYPE_STRING, &roomName, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH, &roomNameLen);
	DatabaseWrapper::SetBind(binds[1], MYSQL_TYPE_STRING, &roomPassword, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH,
		&roomPasswordLen, &IsPasswordNull);

	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), binds.data());

	strcpy_s(roomName, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH, name.c_str());
	roomNameLen = strlen(roomName);
	if (!IsPasswordNull)
	{
		strcpy_s(roomPassword, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH, password.c_str());
		roomPasswordLen = strlen(roomPassword);
	}

	bool result = DatabaseWrapper::ExecuteStmt(stmt);
	if (result == true)
	{
		outid = DatabaseWrapper::Insert_Id(stmt);
	}

	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

bool ChattingRoomRepository::FindChattingRoomById(DatabaseWrapper* wrapper, INT32 roomID, ChattingRoomEntity& outChattingRoom)
{
	MYSQL_BIND in_mysql_bind;
	INT32 id = roomID;
	std::string query = "SELECT * FROM chattingroom WHERE id = ?";

	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_LONG, &id, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);

	std::vector<MYSQL_BIND> binds;
	MYSQL_BIND out_mysql_bind;
	ChattingRoomEntity outEntity;

	bool isPasswordNull = false;

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &outEntity.id, sizeof(INT32));
	binds.push_back(out_mysql_bind);

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_STRING, &outEntity.name, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH);
	binds.push_back(out_mysql_bind);

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_STRING, &outEntity.password, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH, nullptr, &isPasswordNull);
	binds.push_back(out_mysql_bind);

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_TIME, &outEntity.create_at, sizeof(MYSQL_TIME));
	binds.push_back(out_mysql_bind);

	if (!DatabaseWrapper::ExecuteStmt(stmt, binds.data()))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}

	if (DatabaseWrapper::FetchStmt(stmt))
	{
		outChattingRoom = outEntity;
		if (isPasswordNull == true)
		{
			ZeroMemory(&outChattingRoom.password, 256);
		}
	}

	DatabaseWrapper::StmtClose(&stmt);
	return true;
}

bool ChattingRoomRepository::FindAllChatRoom(DatabaseWrapper* wrapper, std::vector<RoomInfoPacket>& outRooms)
{
	std::string query = "SELECT id, chattingroomname FROM chattingroom";

	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), nullptr);

	std::vector<MYSQL_BIND> binds;
	MYSQL_BIND out_mysql_bind;
	ChattingRoomEntity outEntity;


	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &outEntity.id, sizeof(INT32));
	binds.push_back(out_mysql_bind);

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_STRING, &outEntity.name, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH);
	binds.push_back(out_mysql_bind);


	if (!DatabaseWrapper::ExecuteStmt(stmt, binds.data()))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}
	while (DatabaseWrapper::FetchStmt(stmt))
	{
		RoomInfoPacket newData;
		newData.roomID = outEntity.id;
		memcpy_s(newData.roomName, 256, outEntity.name, 256);
		outRooms.push_back(std::move(newData));
	}

	DatabaseWrapper::StmtClose(&stmt);
}

bool ChattingRoomRepository::ExistPasswordRoom(DatabaseWrapper* wrapper, INT32 roomID)
{
	MYSQL_BIND in_mysql_bind;
	INT32 id = roomID;

	std::string query = "SELECT password IS NULL FROM chattingroom WHERE id = ?";

	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_LONG, &id, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);


	MYSQL_BIND out_mysql_bind;
	bool result = false;

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_TINY, &result, sizeof(bool));


	if (!DatabaseWrapper::ExecuteStmt(stmt, &out_mysql_bind))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}
	if (DatabaseWrapper::FetchStmt(stmt))
	{
		return result;
	}
	DatabaseWrapper::StmtClose(&stmt);
	return false;
}

bool ChattingRoomRepository::FindPassword(DatabaseWrapper* wrapper, INT32 roomID, std::string& outPassword)
{
	std::string query = "SELECT password FROM chattingroom WHERE id = ?";
	MYSQL_BIND in_mysql_bind;
	INT32 id = roomID;

	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_LONG, &id, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);

	char roomPassword[ChattingRoomEntity::MAX_NAME_CHAR_LENGTH]{ 0 };
	UINT32 roomPasswordLen = 0;
	bool IsPasswordNull = false;

	MYSQL_BIND out_mysql_bind;
	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_STRING, &roomPassword, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH,
		&roomPasswordLen, &IsPasswordNull);


	if (!DatabaseWrapper::ExecuteStmt(stmt, &out_mysql_bind))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}

	if (DatabaseWrapper::FetchStmt(stmt))
	{
		outPassword.resize(ChattingRoomEntity::MAX_NAME_CHAR_LENGTH);
		if (IsPasswordNull != true)
		{
			memcpy_s(outPassword.data(), ChattingRoomEntity::MAX_NAME_CHAR_LENGTH, roomPassword, ChattingRoomEntity::MAX_NAME_CHAR_LENGTH);
		}
	}
	DatabaseWrapper::StmtClose(&stmt);
	return true;
}

bool ChattingRoomRepository::DestroyChattingById(DatabaseWrapper* wrapper, INT32 roomID)
{
	std::string query = "DELETE FROM chattingroom WHERE id = ?";

	MYSQL_BIND in_mysql_bind;
	INT32 id = roomID;

	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_LONG, &id, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);

	if (!DatabaseWrapper::ExecuteStmt(stmt))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}
	DatabaseWrapper::StmtClose(&stmt);
	return true;
}


