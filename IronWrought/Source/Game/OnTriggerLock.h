#pragma once
#include "LockBehavior.h"

class COnTriggerLock : public CLockBehavior
{
public:
	COnTriggerLock(CGameObject& aParent, const SSettings someSettings);
	~COnTriggerLock() override;

	void ActivateEvent() override;
private:

};