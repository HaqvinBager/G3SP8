#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class ELockInteractionTypes
{
	OnTriggerEnter,
	OnLeftClickDown
};

class CLockBehavior : public CBehavior, public IStringObserver
{
public:
	struct SSettings
	{
		std::string myOnKeyCreateNotify;
		std::string myOnKeyInteractNotify;
		std::string myOnNotify;
		void* myData;
	};

	CLockBehavior(CGameObject& aParent, const SSettings someSettings);
	virtual ~CLockBehavior() override;
	void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void RunEvent();
	virtual void ActivateEvent() = 0;

private:
	void Receive(const SStringMessage& aMessage) override;

	SSettings mySettings;

	int myMaxAmountOfKeys;
	int myAmountOfKeys;

	bool myHasTriggered;

};