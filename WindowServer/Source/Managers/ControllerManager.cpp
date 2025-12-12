#include "MainPCH.h"
#include "Header/Managers/ControllerManager.h"

#include "Controllers/CreateChattingRoomController.h"
#include "Controllers/EntryRoomController.h"
#include "Controllers/EntryRoomCompareController.h"
#include "Controllers/ExitClientController.h"
#include "Controllers/ExitRoomController.h"
#include "Controllers/JoinController.h"
#include "Controllers/LoginController.h"
#include "Controllers/LogoutController.h"
#include "Controllers/RequireRoomInfoController.h"
#include "Controllers/SendMessageController.h"
#include "Managers/RecvManager.h"

SINGLETON_CLASS_CPP(ControllerManager);

ControllerManager::~ControllerManager()
{
	RecvManager::DestroyInstance();
	LoginController::DestroyInstance();
	LogoutController::DestroyInstance();
	JoinController::DestroyInstance();
	ExitClientController::DestroyInstance();
	CreateChattingRoomController::DestroyInstance();
	EntryRoomCompareController::DestroyInstance();
	EntryRoomController::DestroyInstance();
	ExitClientController::DestroyInstance();
	ExitRoomController::DestroyInstance();
	RequireRoomInfoController::DestroyInstance();
	SendMessageController::DestroyInstance();
}

void ControllerManager::InitializeControllers()
{
	RecvManager::CreateInstance();
	LoginController::CreateInstance();
	JoinController::CreateInstance();
	LogoutController::CreateInstance();
	ExitClientController::CreateInstance();
	CreateChattingRoomController::CreateInstance();
	EntryRoomCompareController::CreateInstance();
	EntryRoomController::CreateInstance();
	ExitClientController::CreateInstance();
	ExitRoomController::CreateInstance();
	RequireRoomInfoController::CreateInstance();
	SendMessageController::CreateInstance();
}

