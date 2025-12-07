#include <MainPCH.h>
#include <Header/Managers/RepositoryManager.h>

#include "Repositories/UserRepository.h"

SINGLETON_CLASS_CPP(RepositoryManager)

void RepositoryManager::InitializeRepository()
{
	UserRepository::CreateInstance();
}
