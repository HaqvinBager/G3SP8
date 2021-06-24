#pragma once
#include "LockBehavior.h"
class CKeyBehavior;
class CInstantActivationLock : public CLockBehavior
{
public:
	CInstantActivationLock(CGameObject& aParent, const SSettings someSettings);
	~CInstantActivationLock() override;
	
	void AddKey(CKeyBehavior* aKey) override;
	void RemoveKey(CKeyBehavior* aKey) override;
	void OnKeyActivated(CKeyBehavior* aKey) override;
	void ActivateEvent() override;
};

