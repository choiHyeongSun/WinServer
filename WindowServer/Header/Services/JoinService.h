#pragma once
#include "Repositories/UserRepository.h"

class JoinService
{
private:
	SERVICE_CLASS_H(JoinService);
	INJECTION(UserRepository, userRepository);

public:
	bool JoinUser(std::string username, std::string password, std::string nickname, std::string & resultMsg);

};
