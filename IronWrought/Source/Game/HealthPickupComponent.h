#pragma once
#include "Behavior.h"

class CHealthPickupComponent : public CBehavior
{
public:
	CHealthPickupComponent(CGameObject& aParent, const float& aHealthPickupAmount);
	~CHealthPickupComponent() override;

	void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

public:
	const float& GetHealthPickupAmount() const { return myHealthPickupAmount; }

private:
	float myHealthPickupAmount;
};

