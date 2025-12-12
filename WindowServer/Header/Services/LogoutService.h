#pragma once
#include "Repositories/UserRepository.h"

class LogoutService
{
	SERVICE_CLASS_H(LogoutService);
	INJECTION(UserRepository, userRepository);
public:
	void LogoutProcess(UINT32 localUserID) const;
};
