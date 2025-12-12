#include "MainPCH.h"
#include "Header/Services/EntryRoomService.h"

#include "Utility.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(EntryRoomService)

bool EntryRoomService::PasswordCompare(INT32 userID ,INT32 roomID, std::string password)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	std::string outPassword;

	wrapper->Transaction_Start();
	if (chattingRoomRepository->FindPassword(wrapper.get(), roomID, outPassword))
	{
		outPassword = Utility::ToANSI(outPassword);
		if (outPassword.empty())
		{
			roomMemberRepository->InsertRoomMember(wrapper.get(), userID, roomID);
			if (!wrapper->Transaction_End())
			{
				DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
				return false;
			}
			return true;
		}
		if (password == outPassword)
		{
			roomMemberRepository->InsertRoomMember(wrapper.get(), userID, roomID);
			if (!wrapper->Transaction_End())
			{
				DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
				return false;
			}
			DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
			return true;
		}
	}

	if (!wrapper->Transaction_End())
	{
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return false;
}

bool EntryRoomService::GetRoomMember(INT32 userID, INT32 roomID, RoomMember& outRoomMember)
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
