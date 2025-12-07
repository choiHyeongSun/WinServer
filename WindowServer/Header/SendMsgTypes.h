#pragma once
#include "Pool/PoolingManager.h"

template<class TYPE>
struct alignas(16) PoolingObject
{
	TYPE Data;
};


enum class EMESSAGE_TYPE : UINT32
{
	NONE = 0,
	TEST_PACKET,
	ENTRY_SERVER,
	LOGIN_PACKET,
	JOIN_PACKET,
	SEND_MESSAGE,
	RESULT_MESSAGE,
};

enum class EMESSAGE_RESULT : UINT32
{
	SUCCESS = 0,
	FAILED = 1,
};



#pragma pack(push, 1)
struct PacketHeader
{
	PacketHeader() {}
	PacketHeader(EMESSAGE_TYPE type) : MessageType(type) {}
	INT32 MessageLen = 0;
	EMESSAGE_TYPE MessageType = EMESSAGE_TYPE::NONE;
};
struct EntryServer : public PacketHeader
{
	EntryServer() : PacketHeader(EMESSAGE_TYPE::ENTRY_SERVER) {}
	UINT32 UserLocalId = 0;
};
struct TestPacket : public PacketHeader
{
	TestPacket() : PacketHeader(EMESSAGE_TYPE::TEST_PACKET) {}
	INT32 TestMessage[3000] = { 0 };
};

struct LoginPacket : public PacketHeader
{
	LoginPacket() : PacketHeader(EMESSAGE_TYPE::LOGIN_PACKET) {}
	char UserID[64] = { 0 };
	char Password[64] = { 0 };
};
struct JoinPacket : public PacketHeader
{
	JoinPacket() : PacketHeader(EMESSAGE_TYPE::JOIN_PACKET) {}
	char UserID[64] = { 0 };
	char Password[64] = { 0 };
	char Nickname[64] = { 0 };
};
struct ResultMessage : public PacketHeader
{
	ResultMessage() : PacketHeader(EMESSAGE_TYPE::RESULT_MESSAGE) {}
	EMESSAGE_TYPE PrevMessageType = EMESSAGE_TYPE::NONE;
	EMESSAGE_RESULT ResultType = EMESSAGE_RESULT::SUCCESS;

};
#pragma pack(pop)

class PacketBuilder
{
public:
	static void* PacketBuild(EMESSAGE_TYPE type)
	{
		switch (type)
		{
		case EMESSAGE_TYPE::NONE:
			return PoolingManager<PoolingObject<PacketHeader>>::GenerateObject();
		case EMESSAGE_TYPE::TEST_PACKET:
			return PoolingManager<PoolingObject<TestPacket>>::GenerateObject();
		case EMESSAGE_TYPE::ENTRY_SERVER:
			return PoolingManager<PoolingObject<EntryServer>>::GenerateObject();
		case EMESSAGE_TYPE::LOGIN_PACKET:
			return PoolingManager<PoolingObject<LoginPacket>>::GenerateObject();
		case EMESSAGE_TYPE::JOIN_PACKET:
			return PoolingManager<PoolingObject<JoinPacket>>::GenerateObject();
		case EMESSAGE_TYPE::RESULT_MESSAGE:
			return PoolingManager<PoolingObject<ResultMessage>>::GenerateObject();
		}
		return nullptr;
	}


	static void ReleasePacket(EMESSAGE_TYPE type, void** packet)
	{
		switch (type)
		{
		case EMESSAGE_TYPE::NONE:
			PoolingManager<PoolingObject<PacketHeader>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::TEST_PACKET:
			PoolingManager<PoolingObject<TestPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::ENTRY_SERVER:
			PoolingManager<PoolingObject<EntryServer>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::LOGIN_PACKET:
			PoolingManager<PoolingObject<LoginPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::JOIN_PACKET:
			PoolingManager<PoolingObject<JoinPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::RESULT_MESSAGE:
			PoolingManager<PoolingObject<ResultMessage>>::FreeObject(packet); break;
		}
	}

	template<class T>
	static PoolingObject<T>* PacketBuild()
	{
		return PoolingManager<PoolingObject<T>>::GenerateObject();
	}

	template<class T>
	static void ReleasePacket(PoolingObject<T>** release)
	{
		return PoolingManager<PoolingObject<T>>::FreeObject(release);
	}

	template<typename T>
	static T HtonPacketType(T messageType)
	{
		return static_cast<T>(htonl(static_cast<INT32>(messageType)));
	}

	template<class T>
	static T NtoHPacketType(T messageType)
	{
		return static_cast<T>(ntohl(static_cast<INT32>(messageType)));
	}

	template<class T>
	static const PoolingObject<T>* ConvertToPoolingObject(const char* packet)
	{
		return reinterpret_cast<const PoolingObject<LoginPacket>*>(packet);
	}
};