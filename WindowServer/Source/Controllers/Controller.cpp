#include "MainPCH.h"
#include "Header/Controllers/Controller.h"
#include "Interfaces/ISocketWrapper.h"
#include "Utility.h"

void Controller::SendResult(ISocketWrapper* socketWrapper, EMESSAGE_RESULT resultType, const PacketHeader* packetHeader, std::string msg)
{
	PoolingObject<ResultPacket>* result = PacketBuilder::PacketBuild<ResultPacket>();

	GetResult(resultType, packetHeader, msg, result);
	const char* buffer = reinterpret_cast<const char*>(&result->Data);
	const size_t len = sizeof(ResultPacket);

	socketWrapper->PrivateMessage(packetHeader->UserLocalId, buffer, len, 0);
	PacketBuilder::ReleasePacket<ResultPacket>(&result);

}

void Controller::GetResult(EMESSAGE_RESULT resultType, const PacketHeader* packetHeader,
	std::string msg, PoolingObject<ResultPacket>* outResult)
{
	outResult->Data.PacketType = PacketBuilder::HtonPacketType(outResult->Data.PacketType);
	outResult->Data.PrevMessageType = PacketBuilder::HtonPacketType(packetHeader->PacketType);
	outResult->Data.PacketLen = htonl(sizeof(ResultPacket));
	outResult->Data.UserLocalId = htonl(packetHeader->UserLocalId);
	outResult->Data.ResultType = PacketBuilder::HtonPacketType<EMESSAGE_RESULT>(resultType);

	msg = Utility::ToUTF8(msg);
	memcpy_s((void*)outResult->Data.ResultMsg, 256, (void*)msg.c_str(), msg.size());
}
