#pragma once
#include "Behaviour.h"

class CPickupComponent : public CBehaviour
{
public:
	CPickupComponent(CGameObject& aParent);
	virtual ~CPickupComponent() override;

	virtual void Destroy();

public:
	virtual void OnPickUp() = 0;
};