#pragma once
#include "Behaviour.h"

class CFuseComponent : public CBehaviour
{
public:
	CFuseComponent(CGameObject& aParent);
	~CFuseComponent() override;

	void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
	void OnPickUp();
};