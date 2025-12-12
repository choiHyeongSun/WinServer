#include "MainPCH.h"
#include "Header/Services/ExitClientService.h"

#include "Managers/DatabaseManager.h"

SERVICE_CLASS_CPP(ExitClientService)

void ExitClientService::ExitClientProcess(UINT32 localID)
{
	DatabaseManager::getInstance()->UnRegisterLocalUserID(localID);
};
