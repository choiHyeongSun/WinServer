#pragma once
#include <mysql.h>
#include <shared_mutex>

#include "SendMsgTypes.h"
#include "Entities/ChattingRoomEntity.h"
#include "Interfaces/IRepository.h"

class DatabaseWrapper;

class  ChattingRoomRepository : public IRepository
{
	REPOSITORY_CLASS_H(ChattingRoomRepository)

protected:
	void Initialization() override;

public:
	bool InsertChattingRoom(DatabaseWrapper* wrapper, std::string name, std::string password, INT32 &outid);
	bool FindChattingRoomById(DatabaseWrapper* wrapper, INT32 roomID, ChattingRoomEntity& outChattingRoom);
	bool FindAllChatRoom(DatabaseWrapper* wrapper, std::vector<RoomInfoPacket>& outRooms);
	bool ExistPasswordRoom(DatabaseWrapper* wrapper, INT32 roomID);
	bool FindPassword(DatabaseWrapper* wrapper, INT32 roomID, std::string& outPassword);
	bool DestroyChattingById(DatabaseWrapper* wrapper, INT32 roomID);
};
