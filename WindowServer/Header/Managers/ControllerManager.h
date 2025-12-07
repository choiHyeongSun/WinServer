#pragma once
#include "SendMsgTypes.h"

class ControllerManager
{

private:
	SINGLETON_CLASS_H(ControllerManager);

public:
	~ControllerManager();
	
public:
	void InitializeControllers();
};
