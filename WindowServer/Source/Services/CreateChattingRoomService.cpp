#include "MainPCH.h"
#include "Header/Services/CreateChattingRoomService.h"

#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(CreateChattingRoomService)

bool CreateChattingRoomService::CreateChattingRoom(std::string name, std::string password, INT32& outID,  std::string& outMsg)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	if (chattingRoomRepository->InsertChattingRoom(wrapper.get(), name, password, outID))
	{
		
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return true;
	}
	else
	{
		outMsg = "채팅방 생성 실패 했습니다.";
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}
	
};
