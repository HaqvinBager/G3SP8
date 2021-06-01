#pragma once
#include "LockBehavior.h"

class CInstantActivationLock : public CLockBehavior
{
public:
	CInstantActivationLock(CGameObject& aParent, const SSettings someSettings);
	~CInstantActivationLock() override;

	void Update() override;

	void ActivateEvent() override;
private:

};

