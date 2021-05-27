#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class ELockInteractionTypes
{
	OnTriggerEnter,
	OnLeftClickDown
};

class CLockComponent : public CBehavior, public IStringObserver
{
public:
	CLockComponent(CGameObject& aParent, std::string aCreateRecieveMessage, std::string aDestroyRecieveStringMessage, std::string aSendMessage, void* someData);
	~CLockComponent();
	void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void RunEvent();


private:
	void Receive(const SStringMessage& aMessage) override;

	std::string myCreateReceiveMessage;
	std::string myPickUpReceiveMessage;

	std::string mySendMessage;

	int myMaxAmountOfKeys;
	int myAmountOfKeys;

	bool myHasTriggered;

	void* myData;
};