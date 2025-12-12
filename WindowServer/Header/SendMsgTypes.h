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
	ENTRY_SERVER,
	LOGIN_PACKET,
	JOIN_PACKET,
	SEND_MESSAGE,
	RESULT_MESSAGE,
	LOGOUT_PACKET,
	EXIT_CLIENT,
	CREATE_ROOM,
	SEND_ROOM,
	ENTRY_ROOM_COMPARE,
	ENTRY_ROOM,
	EXIT_ROOM,
	ENTRY_MEMBER,
	EXIT_MEMBER,
	REQUIRE_ROOM_INFO
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
	PacketHeader(EMESSAGE_TYPE type) : PacketType(type) {}
	INT32 PacketLen = 0;
	EMESSAGE_TYPE PacketType = EMESSAGE_TYPE::NONE;
	UINT32 UserLocalId = 0;
};
struct EntryServer : PacketHeader
{
	EntryServer() : PacketHeader(EMESSAGE_TYPE::ENTRY_SERVER) {}
};

struct LoginPacket : PacketHeader
{
	LoginPacket() : PacketHeader(EMESSAGE_TYPE::LOGIN_PACKET) {}
	char UserID[64] = { 0 };
	char Password[64] = { 0 };
};
struct JoinPacket : PacketHeader
{
	JoinPacket() : PacketHeader(EMESSAGE_TYPE::JOIN_PACKET) {}
	char UserID[64] = { 0 };
	char Password[64] = { 0 };
	char Nickname[64] = { 0 };
};
struct SendMessagePacket : PacketHeader
{
	SendMessagePacket() : PacketHeader(EMESSAGE_TYPE::SEND_MESSAGE) {}
	INT32 RoomID = 0;
	INT32 MessageCount = 0;
	UINT32 MsgLen = 0;
};
struct ResultPacket : PacketHeader
{
	ResultPacket() : PacketHeader(EMESSAGE_TYPE::RESULT_MESSAGE) {}
	EMESSAGE_TYPE PrevMessageType = EMESSAGE_TYPE::NONE;
	EMESSAGE_RESULT ResultType = EMESSAGE_RESULT::SUCCESS;
	char ResultMsg[256] = {0};
};
struct LogoutPacket: PacketHeader
{
	LogoutPacket() : PacketHeader(EMESSAGE_TYPE::LOGOUT_PACKET) {}
};
struct ExitClientPacket : PacketHeader
{
	ExitClientPacket() : PacketHeader(EMESSAGE_TYPE::EXIT_CLIENT) {}
};
struct CreateRoomPacket : PacketHeader
{
	CreateRoomPacket() : PacketHeader(EMESSAGE_TYPE::CREATE_ROOM) {}
	char RoomName[256] = { 0 };
	char Password[256] = { 0 };
};
struct SendRoomPacket: PacketHeader
{
	SendRoomPacket() : PacketHeader(EMESSAGE_TYPE::SEND_ROOM) {}
	UINT32 Count = 0;
	UINT32 ArrayBufferLen = 0;
};
struct EntryRoomComparePacket : PacketHeader
{
	EntryRoomComparePacket() : PacketHeader(EMESSAGE_TYPE::ENTRY_ROOM_COMPARE) {}
	UINT32 RoomId = 0;
};
struct EntryRoomPacket : PacketHeader
{
	EntryRoomPacket() : PacketHeader(EMESSAGE_TYPE::ENTRY_ROOM) {}
	UINT32 RoomId = 0;
	char Password[256] = { 0 };
};
struct ExitRoomPacket : PacketHeader
{
	ExitRoomPacket() : PacketHeader(EMESSAGE_TYPE::EXIT_ROOM) {}
	UINT32 RoomId = 0;
};
struct EntryMemberPacket : PacketHeader
{
	EntryMemberPacket() : PacketHeader(EMESSAGE_TYPE::ENTRY_MEMBER) {}
	INT32 RoomID = 0;
	INT32 MemberCount = 0;
	INT32 RoomMemberLen = 0;
};
struct ExitMemberPacket : PacketHeader
{
	ExitMemberPacket() : PacketHeader(EMESSAGE_TYPE::EXIT_MEMBER) {}
	INT32 RoomID = 0;
	INT32 MemberCount = 0;
	INT32 RoomMemberLen = 0;
};
struct RequireRoomInfoPacket : PacketHeader
{
	RequireRoomInfoPacket() : PacketHeader(EMESSAGE_TYPE::REQUIRE_ROOM_INFO) {}
	INT32 RoomId = 0;
};


struct RoomInfoPacket
{
	INT32 roomID;
	char roomName[256] = { 0 };
};
struct RoomMember
{
	INT32 userID;
	char userName[256] = { 0 };
};

struct MessageHeader
{
	INT32 SendUserID = 0;
	char UserName[256] = { 0 };
	INT32 MsgLen = 0;
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
		case EMESSAGE_TYPE::ENTRY_SERVER:
			return PoolingManager<PoolingObject<EntryServer>>::GenerateObject();
		case EMESSAGE_TYPE::LOGIN_PACKET:
			return PoolingManager<PoolingObject<LoginPacket>>::GenerateObject();
		case EMESSAGE_TYPE::JOIN_PACKET:
			return PoolingManager<PoolingObject<JoinPacket>>::GenerateObject();
		case EMESSAGE_TYPE::RESULT_MESSAGE:
			return PoolingManager<PoolingObject<ResultPacket>>::GenerateObject();
		case EMESSAGE_TYPE::LOGOUT_PACKET:
			return PoolingManager<PoolingObject<LogoutPacket>>::GenerateObject();
		case EMESSAGE_TYPE::EXIT_CLIENT:
			return PoolingManager<PoolingObject<ExitClientPacket>>::GenerateObject();
		case EMESSAGE_TYPE::CREATE_ROOM:
			return PoolingManager<PoolingObject<CreateRoomPacket>>::GenerateObject();
		case EMESSAGE_TYPE::SEND_ROOM:
			return PoolingManager<PoolingObject<SendRoomPacket>>::GenerateObject();
		case EMESSAGE_TYPE::ENTRY_ROOM_COMPARE:
			return PoolingManager<PoolingObject<EntryRoomComparePacket>>::GenerateObject();
		case EMESSAGE_TYPE::ENTRY_ROOM:
			return PoolingManager<PoolingObject<EntryRoomPacket>>::GenerateObject();
		case EMESSAGE_TYPE::EXIT_ROOM:
			return PoolingManager<PoolingObject<ExitRoomPacket>>::GenerateObject();
		case EMESSAGE_TYPE::ENTRY_MEMBER:
			return PoolingManager<PoolingObject<EntryMemberPacket>>::GenerateObject();
		case EMESSAGE_TYPE::REQUIRE_ROOM_INFO:
			return PoolingManager<PoolingObject<RequireRoomInfoPacket>>::GenerateObject();

		}
		return nullptr;
	}


	static void ReleasePacket(EMESSAGE_TYPE type, void** packet)
	{
		switch (type)
		{
		case EMESSAGE_TYPE::NONE:
			PoolingManager<PoolingObject<PacketHeader>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::ENTRY_SERVER:
			PoolingManager<PoolingObject<EntryServer>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::LOGIN_PACKET:
			PoolingManager<PoolingObject<LoginPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::JOIN_PACKET:
			PoolingManager<PoolingObject<JoinPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::RESULT_MESSAGE:
			PoolingManager<PoolingObject<ResultPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::LOGOUT_PACKET:
			PoolingManager<PoolingObject<LogoutPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::EXIT_CLIENT:
			PoolingManager<PoolingObject<ExitClientPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::CREATE_ROOM:
			PoolingManager<PoolingObject<CreateRoomPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::SEND_ROOM:
			PoolingManager<PoolingObject<SendRoomPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::ENTRY_ROOM_COMPARE:
			PoolingManager<PoolingObject<EntryRoomComparePacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::ENTRY_ROOM:
			PoolingManager<PoolingObject<EntryRoomPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::EXIT_ROOM:
			PoolingManager<PoolingObject<ExitRoomPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::ENTRY_MEMBER:
			PoolingManager<PoolingObject<EntryMemberPacket>>::FreeObject(packet); break;
		case EMESSAGE_TYPE::REQUIRE_ROOM_INFO:
			PoolingManager<PoolingObject<RequireRoomInfoPacket>>::FreeObject(packet); break;
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