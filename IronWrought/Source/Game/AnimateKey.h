#pragma once
#include "KeyBehavior.h"

class CAnimateKey : public CKeyBehavior
{
public:
	CAnimateKey(CGameObject& aParent, const SSettings& someSettings);
	~CAnimateKey() override;
	void OnPickUp() override;
private:

};