#pragma once
#include "Behavior.h"

class CGameObject;
class CBoxLight;

class CBoxLightComponent : public CBehavior
{
public:
	CBoxLightComponent(CGameObject& aParent, const Vector3& aDirection, const Vector3& aColor, float anIntensity, float aRange = 1.0f, float aWidth = 1.0f, float aHeight = 1.0f);
	~CBoxLightComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	CBoxLight* GetBoxLight() const;

private:
	CBoxLight* myBoxLight;
};

