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
	WCHAR UserID[64] = { 0 };
	WCHAR Password[64] = { 0 };
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
		}
	}
};