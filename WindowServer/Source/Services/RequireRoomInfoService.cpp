#include "MainPCH.h"
#include "Services/RequireRoomInfoService.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(RequireRoomInfoService)

bool RequireRoomInfoService::FindAllRoomMember(INT32 userID, INT32 roomID, std::vector<RoomMember>& roomMember)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	bool result = roomMemberRepository->FindAllRoomMember(wrapper.get(), userID, roomID, roomMember);
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return result;
}

bool RequireRoomInfoService::FindAllMessages(INT32 roomID,
	std::vector<std::pair<MessageHeader, std::string>>& outMessages)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	bool result = messageRepository->FindAllMessage(wrapper.get(), roomID, outMessages);
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return result;
};
