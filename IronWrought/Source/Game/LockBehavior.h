#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class ELockInteractionTypes
{
	OnTriggerEnter,
	OnLeftClickDown
};

class CListenerBehavior;
class CKeyBehavior;

class CLockBehavior : public CBehavior, public IMessageObserver
{
public:
	struct SSettings
	{
		std::string myOnNotifyName;
		int myOnNotify;
		int myOnKeyCreateNotify;
		int myOnKeyInteractNotify;
		void* myData;
	};

	CLockBehavior(CGameObject& aParent, const SSettings someSettings);

	void Register(CListenerBehavior* aListener);
	void Unregister(CListenerBehavior* aListener);

	void AddKey(CKeyBehavior* aKey);
	void RemoveKey(CKeyBehavior* aKey);
	void OnKeyActivated(CKeyBehavior* aKey);

	virtual ~CLockBehavior() override;
	void Destroy() {}

public:
	void Awake() override {}
	void Start() override {}
	virtual void Update() override {}

	void OnEnable() override;
	void OnDisable() override;

	void RunEvent();
	void RunEventEditor();
	virtual void ActivateEvent() = 0;

	const int MaxAmountOfKeys() const { return myMaxAmountOfKeys; }
	const size_t AmountOfKeys() const { return myAmountOfKeys; }

	const bool IsUnlocked() const { return myAmountOfKeys >= myMaxAmountOfKeys; }

	const bool IsTriggered() const { return myHasTriggered; }


private:
	void Receive(const SIDMessage& aMessage) override;

	SSettings mySettings;
	bool myHasSubscribed;
	int myMaxAmountOfKeys;
	size_t myAmountOfKeys;
	std::vector<CListenerBehavior*> myListeners;
	std::vector<CKeyBehavior*> myKeys;



public:
	bool myHasTriggered;

};