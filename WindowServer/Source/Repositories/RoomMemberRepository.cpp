#include "MainPCH.h"
#include "Repositories/RoomMemberRepository.h"

#include "Header/Managers/DatabaseManager.h"
#include "Header/Databases/DatabaseWrapper.h"
#include "Pool/DatabasePool.h"

using namespace std::string_literals;

REPOSITORY_CLASS_CPP(RoomMemberRepository);


void RoomMemberRepository::Initialization()
{
	std::string query =
		"CREATE TABLE IF NOT EXISTS RoomMember(RoomID INT, UserID INT, "s +
		"FOREIGN KEY (RoomID) REFERENCES chattingroom(id) ON DELETE CASCADE, "s +
		"FOREIGN KEY (UserID) REFERENCES users(id) ON DELETE CASCADE);";

	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	wrapper->CreateTable(query.c_str());
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
}

bool RoomMemberRepository::InsertRoomMember(DatabaseWrapper* wrapper, INT32 userID, INT32 roomID)
{
	std::vector<MYSQL_BIND> binds(2);
	std::string query = "INSERT IGNORE INTO RoomMember (RoomID, UserID) VALUES (?, ?)";
	DatabaseWrapper::SetBind(binds[0], MYSQL_TYPE_LONG, &roomID, sizeof(INT32));
	DatabaseWrapper::SetBind(binds[1], MYSQL_TYPE_LONG, &userID, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), binds.data());

	bool result = DatabaseWrapper::ExecuteStmt(stmt);
	mysql_stmt_close(stmt);
	return result;
}

bool RoomMemberRepository::FindAllRoomMember(DatabaseWrapper* wrapper, INT32 userID,INT32 roomID, std::vector<RoomMember>& outRoomMember)
{
	RoomMember outEntity;

	std::vector<MYSQL_BIND> in_binds;
	MYSQL_BIND in_mysql_bind;
	std::string query = "SELECT u.id, u.nickname FROM RoomMember rm JOIN users u ON rm.UserID = u.id WHERE rm.RoomID = ?";
	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_LONG, &roomID, sizeof(INT32));
	in_binds.push_back(in_mysql_bind);
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), in_binds.data());


	std::vector<MYSQL_BIND> out_binds;
	MYSQL_BIND out_mysql_bind;
	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &outEntity.userID, sizeof(INT32));
	out_binds.push_back(out_mysql_bind);

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_STRING, &outEntity.userName, 256);
	out_binds.push_back(out_mysql_bind);

	if (!DatabaseWrapper::ExecuteStmt(stmt, out_binds.data()))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}

	while (DatabaseWrapper::FetchStmt(stmt))
	{
		outRoomMember.push_back(outEntity);
	}
	DatabaseWrapper::StmtClose(&stmt);
	return true;
}

bool RoomMemberRepository::DeleteRoomMember(DatabaseWrapper* wrapper, INT32 userID, INT32 roomID)
{
	std::vector<MYSQL_BIND> binds(2);
	std::string query = "DELETE FROM RoomMember WHERE RoomID = ? AND UserID = ?";
	DatabaseWrapper::SetBind(binds[0], MYSQL_TYPE_LONG, &roomID, sizeof(INT32));
	DatabaseWrapper::SetBind(binds[1], MYSQL_TYPE_LONG, &userID, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), binds.data());

	bool result = DatabaseWrapper::ExecuteStmt(stmt);
	DatabaseWrapper::StmtClose(&stmt);
	return result;
}

INT32 RoomMemberRepository::CountByRoomMember(DatabaseWrapper* wrapper, INT32 roomID)
{
	INT32 count;
	MYSQL_BIND in_mysql_bind;
	std::string query = "SELECT COUNT(*) FROM RoomMember WHERE RoomID = ?";
	DatabaseWrapper::SetBind(in_mysql_bind, MYSQL_TYPE_LONG, &roomID, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), &in_mysql_bind);


	MYSQL_BIND out_mysql_bind;
	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &count, sizeof(INT32));

	if (!DatabaseWrapper::ExecuteStmt(stmt, &out_mysql_bind))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return -1;
	}

	if (DatabaseWrapper::FetchStmt(stmt))
	{
		return count;
	}

	return -1;
}

bool RoomMemberRepository::FindRoomMember(DatabaseWrapper* wrapper, INT32 userID, INT32 roomID, RoomMember& outRoomMember)
{
	std::vector<MYSQL_BIND> binds(2);
	std::string query = "SELECT u.id, u.nickname FROM RoomMember rm JOIN users u ON rm.UserID = u.id WHERE rm.UserID = ? AND rm.RoomID = ?";

	DatabaseWrapper::SetBind(binds[0], MYSQL_TYPE_LONG, &userID, sizeof(INT32));
	DatabaseWrapper::SetBind(binds[1], MYSQL_TYPE_LONG, &roomID, sizeof(INT32));
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), binds.data());

	std::vector<MYSQL_BIND> out_binds;
	MYSQL_BIND out_mysql_bind;
	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_LONG, &outRoomMember.userID, sizeof(INT32));
	out_binds.push_back(out_mysql_bind);

	DatabaseWrapper::SetBind(out_mysql_bind, MYSQL_TYPE_STRING, &outRoomMember.userName, 256);
	out_binds.push_back(out_mysql_bind);

	if (!DatabaseWrapper::ExecuteStmt(stmt, out_binds.data()))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}

	if (DatabaseWrapper::FetchStmt(stmt))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return true;
	}

	DatabaseWrapper::StmtClose(&stmt);
	return false;
}
