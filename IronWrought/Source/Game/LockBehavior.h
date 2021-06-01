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
	virtual void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void RunEvent();
	void RunEventEditor();
	virtual void ActivateEvent() = 0;

	const int MaxAmountOfKeys() const { return myMaxAmountOfKeys; }
	const int AmountOfKeys() const { return myAmountOfKeys; }

private:
	void Receive(const SStringMessage& aMessage) override;

	SSettings mySettings;

	int myMaxAmountOfKeys;
	int myAmountOfKeys;

public:
	bool myHasTriggered;

};