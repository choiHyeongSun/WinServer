#include "MainPCH.h"
#include "Header/Managers/ControllerManager.h"

#include "Controllers/LoginController.h"
#include "Controllers/TestController.h"

SINGLETON_CLASS_CPP(ControllerManager);

ControllerManager::~ControllerManager()
{
	RecvManager::DestroyInstance();
	LoginController::DestroyInstance();
	TestController::DestroyInstance();
}

void ControllerManager::InitializeControllers()
{
	RecvManager::CreateInstance();
	LoginController::CreateInstance();
	TestController::CreateInstance();
}
