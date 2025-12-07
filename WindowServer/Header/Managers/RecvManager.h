#pragma once
#include <mutex>
#include <set>
#include <unordered_set>

#include "SendMsgTypes.h"
#include "Interfaces/IRecvMessage.h"
#include <shared_mutex>

class RecvManager
{

private:
	SINGLETON_CLASS_H(RecvManager)
	std::shared_mutex mutex;

public:
	static void RegisterCallback(EMESSAGE_TYPE type, std::shared_ptr<IRecvMessage> recvMessage);
	static void ExecuteCallback(EMESSAGE_TYPE type, UINT32 localUserID, const PacketHeader* packetHeader, const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper);
	static void UnRegisterCallback(EMESSAGE_TYPE type, std::shared_ptr<IRecvMessage> recvMessage);

private:
	std::unordered_map<EMESSAGE_TYPE, std::vector<std::shared_ptr<IRecvMessage>>> RecvMessageCallbacks;
};
