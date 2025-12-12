#include "MainPCH.h"
#include "Header/Services/LogoutService.h"

#include "Managers/DatabaseManager.h"

SERVICE_CLASS_CPP(LogoutService)

void LogoutService::LogoutProcess(UINT32 localUserID) const
{
	DatabaseManager::getInstance()->UnRegisterUserID(localUserID);
}
;
