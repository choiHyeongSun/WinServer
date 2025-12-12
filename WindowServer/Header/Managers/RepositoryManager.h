#pragma once

class RepositoryManager
{
	SINGLETON_CLASS_H(RepositoryManager)
public:
	~RepositoryManager();

public:
	void InitializeRepository();

private:
};