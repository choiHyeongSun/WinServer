#pragma once
#include "Repositories/UserRepository.h"

class LoginService
{
private:
	SERVICE_CLASS_H(LoginService);
	INJECTION(UserRepository, userRepository);

public:
	bool Login(UINT32 localUserId, std::string username, std::string password);
};
