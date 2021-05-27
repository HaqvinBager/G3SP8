#pragma once
#include "Behavior.h"

class CGameObject;
class CPointLight;

class CPointLightComponent : public CBehavior
{
public:
	CPointLightComponent(CGameObject& aParent, float aRange, DirectX::SimpleMath::Vector3 aColorAndIntensity, float anIntensity);
	~CPointLightComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	CPointLight* GetPointLight() const;

private:
	CPointLight* myPointLight;
	float myRange;
	DirectX::SimpleMath::Vector3 myColor;
	float myIntensity;
};

