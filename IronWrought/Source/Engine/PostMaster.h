#pragma once
#include <vector>
#include <unordered_map>
#include "MessageType.h"
#include "Observer.h"
#include <string>
#include <queue>
#include "PostMasterStructs.h"
#include "PostMasterHelpers.h"

class CPostMaster
{	
	friend class CMainSingleton;

public:
	void Subscribe(const EMessageType aMessageType, IObserver* anObserver);
	void Unsubscribe(const EMessageType aMessageType, IObserver* anObserver);
	void SendLate(const SMessage& aMessage);
	void Send(const SMessage& aMessage);

	void Subscribe(const char*, IStringObserver* anObserver);
	void Unsubscribe(const std::string aMessageType, IStringObserver* anObserver);
	void Send(const SStringMessage& aMessage);

	void Subscribe(const int aMessageID, IMessageObserver* anObserver);
	void Unsubscribe(const int aMessageID, IMessageObserver* anObserver);
	void SendLate(const SIDMessage& aMessage);
	void Send(const SIDMessage& aMessage);

	
	void FlushEvents();

private:
	std::unordered_map<EMessageType, std::vector<IObserver*>> myObserverMap;
	std::unordered_map<std::string, std::vector<IStringObserver*>> myStringObserverMap;
	std::unordered_map<int, std::vector<IMessageObserver*>> myIDMessageObserverMap;



	std::queue<SMessage> myMessageQueue;
	std::queue<SStringMessage> myStringMessageQueue;
	std::queue<SIDMessage> myIDMessageQueue;
private:
	CPostMaster();
	~CPostMaster();
};