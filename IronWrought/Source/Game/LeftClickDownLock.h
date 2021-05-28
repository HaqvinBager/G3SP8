#pragma once
#include "LockBehavior.h"

class CLeftClickDownLock : public CLockBehavior
{
public:
	CLeftClickDownLock(CGameObject& aParent, const SSettings someSettings);
	~CLeftClickDownLock() override;

	void ActivateEvent() override;
private:

};