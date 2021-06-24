#pragma once
#include "ActivationBehavior.h"

class CInstantActivationLock;
class CNotifyLockActivation : public IActivationBehavior
{
public:
	struct SSettings {
		int myLockToNotifyInstanceID;
	};

	CNotifyLockActivation(CGameObject& aParent, const SSettings& someSettings);
	~CNotifyLockActivation() override;

	void OnEnable() override;
	void OnDisable() override;
	void OnActivation() override;

private:
	SSettings mySettings;
	CInstantActivationLock* myLockToNotify;
};