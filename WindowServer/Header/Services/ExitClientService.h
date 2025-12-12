#pragma once
#include "Interfaces/IRecvMessage.h"

class ExitClientService 
{
	SERVICE_CLASS_H(ExitClientService);

public:
	void ExitClientProcess(UINT32 localID);
};