#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class EResponseType
{
	Rotate,
	Move
};

class IResponseBehavior;

class CListenerBehavior : public CBehavior, public IMessageObserver
{
public:
	CListenerBehavior(CGameObject& aParent, const int aReceiveMessage);
	CListenerBehavior(CGameObject& aParent, const int aReceiveMessage, const int aLockInstanceID);
	~CListenerBehavior();

	const int RecieveMessage() const { return myReceiveMessage; }
	
public:
	void Awake() override{}
	void Start() override{}
	void Update() override{}

	void OnEnable() override;
	void OnDisable() override;

	void Register(IResponseBehavior* aBehaviour);
	void TriggerResponses();
	void OnRaiseEvent();

private:
	void Receive(const SIDMessage& aMessage) override;

	int myLockInstanceID;
	std::vector<IResponseBehavior*> myResponses;
	
	int myReceiveMessage;
	bool myHasTriggered;
};

