#pragma once

#include <iostream>
#include <memory>
#include <cmath>
#include <queue>
#include <stack>
#include <vector>
#include <functional>
#include <chrono>
#include <malloc.h>
#include <cstdlib>

#include <chrono>

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")



#define APP_PORT "8080"
#define MAX_LISTEN_COUNT 10

#define MUTEX_CHECK(result) \
	if (result == WAIT_FAILED) \
	{\
	DWORD errorCode = GetLastError();\
	std::cout << "FILE : " <<  __FILE__ << "LINE : " << __LINE__ << __FUNCTION__  << " Mutex Error : " << errorCode << std::endl;\
	exit(-1); \
	}

#define SINGLETON_CLASS_H(CLASS_NAME) \
private: \
	CLASS_NAME() {} \
	static std::shared_ptr<CLASS_NAME> instance; \
public: \
	static std::shared_ptr<CLASS_NAME> getInstance() \
	{ \
		return instance; \
	}\
	static void CreateInstance(); \
	static void DestroyInstance(); \



#define SINGLETON_CLASS_CPP(CLASS_NAME) \
	std::shared_ptr<CLASS_NAME> CLASS_NAME::instance = nullptr; \
		\
	void CLASS_NAME::CreateInstance()\
	{ \
		if (instance == nullptr) \
		{ \
			instance = std::shared_ptr<CLASS_NAME>(new CLASS_NAME()); \
		} \
	} \
	void CLASS_NAME::DestroyInstance()\
	{ \
		instance.reset(); \
	} \

#define SERVICE_CLASS_H(CLASS_NAME) \
private: \
	CLASS_NAME() {} \
	static std::shared_ptr<CLASS_NAME> instance; \
public: \
	static std::shared_ptr<CLASS_NAME> getInstance() \
	{ \
		return instance; \
	}\
	static void CreateInstance(); \
	static void DestroyInstance(); \



#define SERVICE_CLASS_CPP(CLASS_NAME) \
	std::shared_ptr<CLASS_NAME> CLASS_NAME::instance = nullptr; \
		\
	void CLASS_NAME::CreateInstance()\
	{ \
		if (instance == nullptr) \
		{ \
			instance = std::shared_ptr<CLASS_NAME>(new CLASS_NAME()); \
		} \
	} \
	void CLASS_NAME::DestroyInstance()\
	{ \
		instance.reset(); \
	} \


	
#define CONTROLLER_CLASS_H(CLASS_NAME, TYPE) \
private: \
	CLASS_NAME() {} \
	static std::shared_ptr<CLASS_NAME> instance; \
	const EMESSAGE_TYPE messageType = TYPE;\
public: \
	static std::shared_ptr<CLASS_NAME> getInstance() \
	{ \
		return instance; \
	}\
	static void CreateInstance(); \
	static void DestroyInstance(); \



#define CONTROLLER_CLASS_CPP(CLASS_NAME) \
	std::shared_ptr<CLASS_NAME> CLASS_NAME::instance = nullptr; \
		\
	void CLASS_NAME::CreateInstance()\
	{ \
		if (instance == nullptr) \
		{ \
			instance = std::shared_ptr<CLASS_NAME>(new CLASS_NAME()); \
			RecvManager::RegisterCallback(instance->messageType, instance);\
		} \
	} \
	void CLASS_NAME::DestroyInstance()\
	{ \
		instance.reset(); \
	} \


#define REPOSITORY_CLASS_H(CLASS_NAME) \
private: \
	CLASS_NAME() {} \
	static std::shared_ptr<CLASS_NAME> instance; \
public: \
	static std::shared_ptr<CLASS_NAME> getInstance() \
	{ \
		return instance; \
	}\
	static void CreateInstance(); \
	static void DestroyInstance(); \



#define REPOSITORY_CLASS_CPP(CLASS_NAME) \
	std::shared_ptr<CLASS_NAME> CLASS_NAME::instance = nullptr; \
		\
	void CLASS_NAME::CreateInstance()\
	{ \
		if (instance == nullptr) \
		{ \
			instance = std::shared_ptr<CLASS_NAME>(new CLASS_NAME()); \
			instance->Initialization();\
			instance->BindStmt();\
		} \
	} \
	void CLASS_NAME::DestroyInstance()\
	{ \
		instance.reset(); \
	} \



#define PACKET_CHECKER(PACKET_HEADER) \
	if (PACKET_HEADER == nullptr) return;\
	if (messageType != PACKET_HEADER->MessageType) return;\


#define INJECTION(CLASS_NAME, VALUE_NAME) \
private:\
	std::shared_ptr<CLASS_NAME> VALUE_NAME = CLASS_NAME::getInstance()


