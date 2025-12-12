#pragma once
#include "SendMsgTypes.h"
#include "Interfaces/IRepository.h"

class DatabaseWrapper;

class MessageRepository : public IRepository
{
	REPOSITORY_CLASS_H(MessageRepository)

protected:
	virtual void Initialization() override;
	
public:
	bool InsertMessage(DatabaseWrapper* wrapper, INT32 roomID, INT32 userID, char* msg, INT32 len);
	bool FindAllMessage(DatabaseWrapper* wrapper, INT32 roomId, std::vector<std::pair<MessageHeader, std::string>> & outMessageHeader);
};
