#include "MainPCH.h"
#include "Header/Services/LoginService.h"

#include "Utility.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(LoginService)

bool LoginService::Login(UINT32 localUserId ,std::string username, std::string password)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();

	UserEntity userEntity;

	if (userRepository->FindUserByUsername(wrapper, username, userEntity))
	{
		if (userEntity.password == password)
		{
			DatabaseManager::getInstance()->SetUserID(localUserId, userEntity.userID);
			DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
			return true;
		}
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return false;
}

bool LoginService::FindAllChatRoom(std::vector<RoomInfoPacket>& outRooms)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	bool result = chattingRoomRepository->FindAllChatRoom(wrapper.get(), outRooms);
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
	return result;
}
