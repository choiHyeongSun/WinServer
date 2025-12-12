#include "MainPCH.h"
#include "Header/Managers/ServiceManager.h"

#include "Services/CreateChattingRoomService.h"
#include "Services/EntryRoomCompareService.h"
#include "Services/EntryRoomService.h"
#include "Services/ExitClientService.h"
#include "Services/ExitRoomService.h"
#include "Services/JoinService.h"
#include "Services/LoginService.h"
#include "Services/LogoutService.h"
#include "Services/RequireRoomInfoService.h"
#include "Services/SendMessageService.h"


SINGLETON_CLASS_CPP(ServiceManager)

ServiceManager::~ServiceManager()
{
	LoginService::DestroyInstance();
	JoinService::DestroyInstance();
	LogoutService::DestroyInstance();
	ExitClientService::DestroyInstance();
	CreateChattingRoomService::DestroyInstance();
	EntryRoomCompareService::DestroyInstance();
	EntryRoomService::DestroyInstance();
	ExitRoomService::DestroyInstance();
	RequireRoomInfoService::DestroyInstance();
	SendMessageService::DestroyInstance();
}

void ServiceManager::InitializationService()
{
	LoginService::CreateInstance();
	JoinService::CreateInstance();
	LogoutService::CreateInstance();
	ExitClientService::CreateInstance();
	CreateChattingRoomService::CreateInstance();
	EntryRoomCompareService::CreateInstance();
	EntryRoomService::CreateInstance();
	ExitRoomService::CreateInstance();
	RequireRoomInfoService::CreateInstance();
	SendMessageService::CreateInstance();
}
