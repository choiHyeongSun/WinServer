#pragma once

struct UserEntity
{
public:
	static constexpr INT32 MAX_CHAR_LENGTH = 256;

	INT32 userID;
	char username[MAX_CHAR_LENGTH] = {0};
	char password[MAX_CHAR_LENGTH] = {0};
	char nickname[MAX_CHAR_LENGTH] = {0};
	MYSQL_TIME created_at;
};