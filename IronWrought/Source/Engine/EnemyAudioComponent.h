#pragma once
#include "Component.h"
#include <EnemyComponent.h>

class CGameObject;
class CEnemyComponent;

class CEnemyAudioComponent : public CComponent
{
public:
	CEnemyAudioComponent(CGameObject& aParent);
	~CEnemyAudioComponent() override;
	void Awake() override;
	void Start() override;
	void Update() override;

private:
	CEnemyComponent* myEnemyComponent;
	CEnemyComponent::EBehaviour myStateLastFrame;
	float myTimer;
};

