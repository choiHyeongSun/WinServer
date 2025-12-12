#pragma once

struct ChattingRoomEntity
{
	static constexpr INT32 MAX_NAME_CHAR_LENGTH = 256;

	INT32 id = 0;
	char name[MAX_NAME_CHAR_LENGTH] = { 0 };
	char password[MAX_NAME_CHAR_LENGTH] = { 0 };
	MYSQL_TIME create_at;
};