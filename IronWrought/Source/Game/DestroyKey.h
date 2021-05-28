#pragma once
#include "KeyBehavior.h"

class CDestroyKey : public CKeyBehavior
{
public:
	CDestroyKey(CGameObject& aParent, const SSettings& someSettings);
	~CDestroyKey() override;
	void OnPickUp() override;
private:

};