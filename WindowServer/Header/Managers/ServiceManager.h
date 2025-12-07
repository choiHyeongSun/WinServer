#pragma once

class ServiceManager
{
private:
	SINGLETON_CLASS_H(ServiceManager);
	~ServiceManager();

public:
	void InitializationService();
};
