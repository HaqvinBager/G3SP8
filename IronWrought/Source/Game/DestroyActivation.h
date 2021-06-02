#pragma once
#include "ActivationBehavior.h"

class CDestroyActivation : public IActivationBehavior
{
public:

	CDestroyActivation(CGameObject& aParent);
	~CDestroyActivation() override;
	void OnActivation() override;
private:

};