#pragma once
#include "Repositories/MessageRepository.h"

class SendMessageService {
	SERVICE_CLASS_H(SendMessageService);
	INJECTION(MessageRepository, messageRepository);

public:
	bool InsertMessage(INT32 roomID, INT32 userID, char* msg, INT32 len);
};
