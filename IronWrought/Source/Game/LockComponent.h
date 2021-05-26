#pragma once
#include "Behaviour.h"
#include "Observer.h"

class CLockComponent : public CBehaviour, public IStringObserver
{
public:
	CLockComponent(CGameObject& aParent, std::string aCreateListenStringMessage, std::string aDestroyListenStringMessage, std::string aSendStringMessage, void* someData);
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

	std::string myCreateListenStringMessage;
	std::string myDestroyListenStringMessage;
	std::string mySendStringMessage;

	int myMaxAmountOfKeys;
	int myAmountOfKeys;

	bool myHasTriggered;

	void* myData;
};