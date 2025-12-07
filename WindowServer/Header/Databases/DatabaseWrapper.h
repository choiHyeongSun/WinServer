#pragma once
#include <mysql.h>
#include <shared_mutex>

namespace std
{
	class mutex;
}

class DatabaseWrapper
{
private:
	DatabaseWrapper() {}
public:
	~DatabaseWrapper();


public:
	static std::shared_ptr<DatabaseWrapper> CreateDatabase();
	static void SetBind(MYSQL_BIND& bind, enum_field_types bufferType, void* buffer, INT32 bufferLen, UINT32* realLen);
	static void SetBind(MYSQL_BIND& bind, enum_field_types bufferType, void* buffer, INT32 bufferLen);
	static bool ExecuteStmt(MYSQL_STMT* mysql_stmt);
	static bool ExecuteStmt(MYSQL_STMT* mysql_stmt, std::vector<MYSQL_BIND>& output);
	static bool FetchStmt(MYSQL_STMT* mysql_stmt);
	static bool ReleaseStmtResult(MYSQL_STMT* mysql_stmt);
	static void StmtClose(MYSQL_STMT** mysql_stmt);

public:

	void ConnectDB(const char* ip, const char* user, const char* password, const char* db, INT32 port);

	void CreateTable(const char * query) const;
	void Transaction_Start() const;
	void Transaction_End() const;

	void SendQuery(const char* query);

	MYSQL_STMT* StmtBind(const char* query, std::vector<MYSQL_BIND>& binds) const;

private:
	MYSQL Connector;
	MYSQL* ConnectorPtr = nullptr;

};

