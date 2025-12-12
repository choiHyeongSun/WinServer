#include "MainPCH.h"
#include "Header/Services/ExitRoomService.h"

#include "Databases/DatabaseWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(ExitRoomService)

bool ExitRoomService::ExitRoom(INT32 userID ,UINT32 roomID)
{

	bool result = false;
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	wrapper->Transaction_Start();
	INT32 count = roomMemberRepository->CountByRoomMember(wrapper.get(), roomID);
	if (count != -1)
	{
		if (count == 1)
		{
			result = chattingRoomRepository->DestroyChattingById(wrapper.get(), roomID);
		}

		roomMemberRepository->DeleteRoomMember(wrapper.get(), userID, roomID);
		if (!wrapper->Transaction_End())
		{
			DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
			return false;
		}
	}
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return result;
};

bool ExitRoomService::GetRoomMember(INT32 userID, INT32 roomID, RoomMember& outRoomMember)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	if (roomMemberRepository->FindRoomMember(wrapper.get(), userID, roomID, outRoomMember))
	{
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return true;
	}
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return false;
};