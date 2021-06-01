#pragma once
#include "LockBehavior.h"

class COnLookLock : public CLockBehavior
{
public:
	COnLookLock(CGameObject& aParent, const SSettings someSettings);
	~COnLookLock() override;

	void ActivateEvent() override;
private:

};

