#include "MainPCH.h"
#include "Header/Managers/RecvManager.h"

SINGLETON_CLASS_CPP(RecvManager);

void RecvManager::RegisterCallback(EMESSAGE_TYPE type, std::shared_ptr<IRecvMessage> recvMessage)
{
	std::unique_lock<std::shared_mutex> lock(instance->mutex);
	if (instance->RecvMessageCallbacks.find(type) == instance->RecvMessageCallbacks.cend())
	{
		instance->RecvMessageCallbacks.insert({ type, std::vector<std::shared_ptr<IRecvMessage>>() });
	}
	instance->RecvMessageCallbacks[type].push_back(recvMessage);
}

void RecvManager::ExecuteCallback(EMESSAGE_TYPE type, UINT32 localUserID, const PacketHeader* packetHeader,
	const char* packet, INT32 MsgLen, ISocketWrapper* socketWrapper)
{
	//스냅샷 기법으로 안전하게 콜백 실행
	std::vector<IRecvMessage*> executeCallback;
	{
		std::unique_lock<std::shared_mutex> lock(instance->mutex);
		if (instance->RecvMessageCallbacks.find(type) == instance->RecvMessageCallbacks.cend())
			return;

		for (const auto& callback : instance->RecvMessageCallbacks[type])
		{
			executeCallback.push_back(callback.get());
		}
	}

	for (const auto& callback : executeCallback)
	{
		callback->OnRecvMessage(localUserID, packetHeader, packet, MsgLen, socketWrapper);
	}
}

void RecvManager::UnRegisterCallback(EMESSAGE_TYPE type, std::shared_ptr<IRecvMessage> recvMessage)
{
	std::unique_lock<std::shared_mutex> lock(instance->mutex);
	if (instance->RecvMessageCallbacks.find(type) == instance->RecvMessageCallbacks.cend())
		return;

	auto iter = std::find(instance->RecvMessageCallbacks[type].begin(), instance->RecvMessageCallbacks[type].end(), recvMessage);
	instance->RecvMessageCallbacks[type].erase(iter);
}
