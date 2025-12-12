#include "MainPCH.h"
#include "Header/Databases/DatabaseWrapper.h"

DatabaseWrapper::~DatabaseWrapper()
{
	if (ConnectorPtr != nullptr)
	{
		mysql_close(ConnectorPtr);
	} 
}

std::shared_ptr<DatabaseWrapper> DatabaseWrapper::CreateDatabase()
{
	std::shared_ptr<DatabaseWrapper> databaseWrapper = std::shared_ptr<DatabaseWrapper>(new DatabaseWrapper());
	return databaseWrapper;
}



void DatabaseWrapper::ConnectDB(const char* ip, const char* user, const char* password, const char* db, INT32 port)
{
	mysql_init(&Connector);
	ConnectorPtr = mysql_real_connect(&Connector, ip, user, password, nullptr, port, nullptr, CLIENT_MULTI_STATEMENTS);
	if (ConnectorPtr == nullptr)
	{
		std::cout << "DB Connection Error : " << mysql_error(&Connector) << std::endl;
		exit(-1);
	}

	std::string query = \
		" CREATE DATABASE IF NOT EXISTS " + std::string(db) + \
		" DEFAULT CHARACTER SET utf8mb4 " + \
		" DEFAULT COLLATE utf8mb4_0900_ai_ci; ";
	if (mysql_query(ConnectorPtr, query.c_str()))
	{
		std::cout << "DB Creation failed: " << mysql_error(ConnectorPtr) << std::endl;
		exit(-1);
	}

	if (mysql_select_db(ConnectorPtr, db))
	{
		std::cout << "DB Selection Error : " << mysql_error(ConnectorPtr) << std::endl;
		exit(-1);
	}
}

void DatabaseWrapper::CreateTable(const char* query) const
{
	if (mysql_query(ConnectorPtr ,query))
	{
		std::cout << "Table Creation failed: " << mysql_error(ConnectorPtr) << std::endl;
	}
}


void DatabaseWrapper::Transaction_Start() const
{
	mysql_autocommit(ConnectorPtr, false);
}


bool DatabaseWrapper::Transaction_End() const
{
	if (mysql_commit(ConnectorPtr))
	{
		std::cout << "commit failed : " << mysql_error(ConnectorPtr) << std::endl;
		mysql_rollback(ConnectorPtr);
		mysql_autocommit(ConnectorPtr, true);
		return false;
	}
	mysql_autocommit(ConnectorPtr, true);
	return true;
}

void DatabaseWrapper::SetAutoCommit() const
{
	mysql_autocommit(ConnectorPtr, true);
}


void DatabaseWrapper::RollBack() const
{
	mysql_rollback(ConnectorPtr);
}

void DatabaseWrapper::SendQuery(const char* query)
{
	if (mysql_query(ConnectorPtr, query))
	{
		std::cout << "ExecuteStmt Error : " << mysql_error(ConnectorPtr) << std::endl;
	}
}

MYSQL_STMT* DatabaseWrapper::StmtBind(const char* query , MYSQL_BIND* binds) const
{
	MYSQL_STMT* mysql_stmt = mysql_stmt_init(ConnectorPtr);
	if (mysql_stmt == nullptr)
	{
		std::cout << "stmt init error : " << mysql_error(ConnectorPtr) << std::endl;
		return nullptr;
	}

	if (mysql_stmt_prepare(mysql_stmt, query, strlen(query)))
	{
		std::cout << "stmt_prepare erorr : " << mysql_stmt_error(mysql_stmt) << std::endl;
		return nullptr;
	}

	if (binds != nullptr)
	{
		if (mysql_stmt_bind_param(mysql_stmt, binds))
		{
			std::cout << "stmt_prepare erorr : " << mysql_stmt_error(mysql_stmt) << std::endl;
			return nullptr;
		}
	}

	return mysql_stmt;
}

bool DatabaseWrapper::ExecuteStmt(MYSQL_STMT* mysql_stmt)
{
	if (mysql_stmt_execute(mysql_stmt))
	{
		std::cout << "ExecuteStmt Error : " << mysql_stmt_error(mysql_stmt) << std::endl;
		return false;
	}
	return true;
}


bool DatabaseWrapper::ExecuteStmt(MYSQL_STMT* mysql_stmt, MYSQL_BIND* output)
{
	if (mysql_stmt_execute(mysql_stmt))
	{
		std::cout << "ExecuteStmt Error : " << mysql_stmt_error(mysql_stmt) << std::endl;
		return false;
	}

	if (mysql_stmt_store_result(mysql_stmt))
	{
		std::cout << "mysql_stmt_store_result error : "  << mysql_stmt_error(mysql_stmt) << std::endl;
		return false;
	}

	if (mysql_stmt_bind_result(mysql_stmt, output))
	{
		std::cout << "mysql_stmt_bind_result error : " << mysql_stmt_error(mysql_stmt) << std::endl;
		return false;
	}
	return true;
}

UINT64 DatabaseWrapper::Insert_Id(MYSQL_STMT* mysql_stmt)
{
	return mysql_stmt_insert_id(mysql_stmt);
}

bool DatabaseWrapper::FetchStmt(MYSQL_STMT* mysql_stmt)
{
	int result = mysql_stmt_fetch(mysql_stmt);

	if (result == 0) return true;
	if (result == 101) return true;
	if (result == MYSQL_NO_DATA) return false;
	std::cout << "mysql_stmt_fetch error : " << mysql_stmt_error(mysql_stmt) << std::endl;
	return false;
} 

bool DatabaseWrapper::ReleaseStmtResult(MYSQL_STMT* mysql_stmt)
{
	if (mysql_stmt_free_result(mysql_stmt))
	{
		std::cout << "mysql_stmt_free_result error : " << mysql_stmt_error(mysql_stmt) << std::endl;
		return false;
	}
	return true;
}

void DatabaseWrapper::StmtClose(MYSQL_STMT** mysql_stmt)
{
	mysql_stmt_close(*mysql_stmt);
	mysql_stmt = nullptr;
}


void DatabaseWrapper::SetBind(MYSQL_BIND& bind, enum_field_types bufferType, void* buffer, INT32 bufferLen)
{
	ZeroMemory(&bind, sizeof(bind));
	bind.buffer_type = bufferType;
	bind.buffer = buffer;
	bind.buffer_length = bufferLen;
}


void DatabaseWrapper::SetBind(MYSQL_BIND& bind, enum_field_types bufferType, void* buffer, INT32 bufferLen, UINT32 * realLen)
{
	ZeroMemory(&bind, sizeof(bind));
	bind.buffer_type = bufferType;
	bind.buffer = buffer;
	bind.buffer_length = bufferLen;
	bind.length = reinterpret_cast<unsigned long*>(realLen);
}

void DatabaseWrapper::SetBind(MYSQL_BIND& bind, enum_field_types bufferType, void* buffer, INT32 bufferLen,
	UINT32* realLen, bool* isNull)
{
	ZeroMemory(&bind, sizeof(bind));
	bind.buffer_type = bufferType;
	bind.buffer = buffer;
	bind.buffer_length = bufferLen;
	bind.length = reinterpret_cast<unsigned long*>(realLen);
	bind.is_null = isNull;
}