#pragma once

class IRepository
{
public:
	virtual ~IRepository() = default;

protected:
	virtual void Initialization() = 0;
};