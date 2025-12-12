#include <MainPCH.h>
#include <Header/Managers/RepositoryManager.h>

#include "Repositories/ChattingRoomRepository.h"
#include "Repositories/MessageRepository.h"
#include "Repositories/RoomMemberRepository.h"
#include "Repositories/UserRepository.h"

SINGLETON_CLASS_CPP(RepositoryManager)

RepositoryManager::~RepositoryManager()
{
	UserRepository::DestroyInstance();
	ChattingRoomRepository::DestroyInstance();
	RoomMemberRepository::DestroyInstance();
	MessageRepository::DestroyInstance();
}

void RepositoryManager::InitializeRepository()
{
	UserRepository::CreateInstance();
	ChattingRoomRepository::CreateInstance();
	RoomMemberRepository::CreateInstance();
	MessageRepository::CreateInstance();
}
