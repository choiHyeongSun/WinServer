#pragma once
#include "SendMsgTypes.h"
#include "Databases/DatabaseWrapper.h"
#include "Interfaces/IRepository.h"

class RoomMemberRepository : public IRepository
{
	REPOSITORY_CLASS_H(RoomMemberRepository)

protected:
	void Initialization() override;
public:
	bool InsertRoomMember(DatabaseWrapper* wrapper, INT32 userID, INT32 roomID);
	bool FindAllRoomMember(DatabaseWrapper* wrapper, INT32 userID,INT32 roomID, std::vector<RoomMember>& outRoomMember);
	bool DeleteRoomMember(DatabaseWrapper* wrapper, INT32 userID, INT32 roomID);
	INT32 CountByRoomMember(DatabaseWrapper* wrapper, INT32 roomID);
	bool  FindRoomMember(DatabaseWrapper* wrapper, INT32 userID, INT32 roomID, RoomMember& outRoomMember);

};

