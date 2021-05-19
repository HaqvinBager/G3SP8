#pragma once
#include "PickupComponent.h"

class CFuseComponent : public CPickupComponent
{
public:
	CFuseComponent(CGameObject& aParent);
	~CFuseComponent() override;

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
	void OnPickUp() override;
};