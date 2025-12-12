#include "MainPCH.h"
#include "Header/Services/JoinService.h"

#include "SendMsgTypes.h"
#include "Databases/DatabaseWrapper.h"
#include "Managers/DatabaseManager.h"
#include "Pool/DatabasePool.h"

SERVICE_CLASS_CPP(JoinService)

bool JoinService::JoinUser(std::string username, std::string password, std::string nickname, std::string& resultMsg)
{
	std::shared_ptr<DatabaseWrapper> wrapper = DatabaseManager::GetDatabasePool()->PopWrapper();
	resultMsg = "";

	wrapper->Transaction_Start();
	if (userRepository->IsExistUsername(wrapper, username))
	{
		resultMsg = "같은 ID가 존재 합니다.";
		wrapper->RollBack();
		wrapper->SetAutoCommit();
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}

	if (userRepository->IsExistUserNickname(wrapper, username))
	{
		resultMsg = "같은 닉네임이 존재합니다.";
		wrapper->RollBack();
		wrapper->SetAutoCommit();
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}

	if (!userRepository->InsertUser(wrapper, username, password, nickname))
	{
		resultMsg = "요청에 실패 하였습니다.";
		wrapper->RollBack();
		wrapper->SetAutoCommit();
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}
	if (!wrapper->Transaction_End())
	{
		resultMsg = "요청에 실패 하였습니다.";
		DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);
		return false;
	}
	resultMsg = "계정 생성에 성공하였습니다.";
	DatabaseManager::GetDatabasePool()->PushWrapper(wrapper);

	return true;
}