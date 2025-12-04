#pragma once

class ControllerManager
{
	SINGLETON_CLASS_H(ControllerManager);
public:
	~ControllerManager();
	
public:
	void InitializeControllers();
};