#include "MainPCH.h"
#include "Header/Managers/ServiceManager.h"

#include "Services/JoinService.h"
#include "Services/LoginService.h"


SINGLETON_CLASS_CPP(ServiceManager)

ServiceManager::~ServiceManager()
{
	LoginService::DestroyInstance();
	JoinService::DestroyInstance();
}

void ServiceManager::InitializationService()
{
	LoginService::CreateInstance();
	JoinService::CreateInstance();
}
