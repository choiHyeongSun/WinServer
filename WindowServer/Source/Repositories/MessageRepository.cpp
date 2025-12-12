#include "MainPCH.h"
#include "Header/Repositories/MessageRepository.h"

#include "Databases/DatabaseWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

using namespace std::string_literals;

REPOSITORY_CLASS_CPP(MessageRepository)

void MessageRepository::Initialization()
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	std::string query =
		" CREATE TABLE ChatMessage(\
		  ID INT AUTO_INCREMENT PRIMARY KEY, \
		  RoomID INT, UserID INT, \
		  FOREIGN KEY (RoomID) REFERENCES chattingroom(id) ON DELETE CASCADE , \
		  FOREIGN KEY (UserID) REFERENCES users(id) , \
		  message TEXT NOT NULL\
		  );";

	wrapper->CreateTable(query.c_str());
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
}

bool MessageRepository::InsertMessage(DatabaseWrapper* wrapper, INT32 roomID, INT32 userID, char* msg, INT32 len)
{
	std::vector<MYSQL_BIND> binds(3);
	std::string query = "INSERT IGNORE INTO ChatMessage (RoomID, UserID, Message) VALUES (?, ?, ?)";
	DatabaseWrapper::SetBind(binds[0], MYSQL_TYPE_LONG, &roomID, sizeof(INT32));
	DatabaseWrapper::SetBind(binds[1], MYSQL_TYPE_LONG, &userID, sizeof(INT32));
	DatabaseWrapper::SetBind(binds[2], MYSQL_TYPE_STRING, msg, len);
	MYSQL_STMT* stmt = wrapper->StmtBind(query.c_str(), binds.data());

	bool result = DatabaseWrapper::ExecuteStmt(stmt);
	mysql_stmt_close(stmt);
	return result;
}

bool MessageRepository::FindAllMessage(DatabaseWrapper* wrapper, INT32 roomId, std::vector<std::pair<MessageHeader, std::string>>&  outMessageHeader)
{
	MYSQL_BIND in_bind;
	std::string query = "SELECT u.id ,u.nickname, cm.message FROM ChatMessage cm JOIN users u ON cm.UserID = u.id  WHERE cm.RoomID = ?";
	DatabaseWrapper::SetBind(in_bind, MYSQL_TYPE_LONG, &roomId, sizeof(INT32));
	MYSQL_STMT * stmt = wrapper->StmtBind(query.c_str(), &in_bind);

	MessageHeader msgHeader;
	UINT32 real_len;
	std::vector<char> msgBuffer;
	std::vector<MYSQL_BIND> out_binds(3);
	
	DatabaseWrapper::SetBind(out_binds[0], MYSQL_TYPE_LONG, &msgHeader.SendUserID, sizeof(INT32));
	DatabaseWrapper::SetBind(out_binds[1], MYSQL_TYPE_STRING, msgHeader.UserName, 256);
	DatabaseWrapper::SetBind(out_binds[2], MYSQL_TYPE_STRING, nullptr, 0, &real_len);

	if (!DatabaseWrapper::ExecuteStmt(stmt, out_binds.data()))
	{
		DatabaseWrapper::StmtClose(&stmt);
		return false;
	}

	while (DatabaseWrapper::FetchStmt(stmt))
	{
		std::vector<char> msgBuffer(real_len, 0);
		MYSQL_BIND text_bind = {0};
		DatabaseWrapper::SetBind(text_bind, MYSQL_TYPE_STRING, msgBuffer.data(), real_len, &real_len);
		mysql_stmt_fetch_column(stmt, &text_bind, 2, 0);
		std::string messageBody(msgBuffer.data(), real_len);
		outMessageHeader.push_back({ msgHeader , std::move(messageBody) });
	}
};

