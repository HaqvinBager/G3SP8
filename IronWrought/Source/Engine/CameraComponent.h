#pragma once
#include "Component.h"
#include "PerlinNoise.h"
#include <FastNoise\FastNoise.h>

class CGameObject;
class CSpriteInstance;

class CCameraComponent : public CComponent
{
public:
	static constexpr float SP8_FOV = 59.5f;

	CCameraComponent(CGameObject& aParent, const float aFoV = 70.0f/*, float aNearPlane = 0.3f, float aFarPlane = 10000.0f, DirectX::SimpleMath::Vector2 aResolution = {1600.f, 900.f}*/);
	~CCameraComponent();


	void Awake() override;
	void Start() override;
	void Update() override;

	const DirectX::SimpleMath::Matrix& GetProjection() const;
	float GetFoV();

	void SetTrauma(float aValue);
	void SetStartingRotation(DirectX::SimpleMath::Vector3 aRotation);
	void SetFoV(float aFoV);

	void Fade(bool aShouldFadeIn, const float& aFadeDuration = 1.0f, const bool& aFadeIsPermanent = false);
	const bool IsFading() const;
	void EmplaceSprites(std::vector<CSpriteInstance*>& someSprites) const;

	const Matrix& GetViewMatrix();
	const Matrix& GetShakenMatrix();
	const Vector3& GetShakeVector() const;

	const DirectX::BoundingFrustum GetViewFrustum();

	void SetShakeDecay(float aDecayInSeconds);
	void SetShakeSpeed(float aSpeed);
	void SetMaxShakeRotation(const Vector3& aMaxRotation);
	void SetTestTrauma(float aTrauma);

private:
	void Shake();

	enum class ECameraState
	{
		Default,
		FadeIn,
		FadeOut,

		Count
	};

private:
	CSpriteInstance* myFadingPlane;
	DirectX::SimpleMath::Matrix myProjection;
	DirectX::SimpleMath::Matrix myView;
	DirectX::SimpleMath::Matrix myShakenMatrix;
	DirectX::BoundingFrustum myViewFrustum;
	
	DirectX::SimpleMath::Vector3 myStartingRotation;
	DirectX::SimpleMath::Vector3 myMaxShakeRotation;
	DirectX::SimpleMath::Vector3 myShakeVector;
	/*PerlinNoise myNoise;*/
	FastNoise::SmartNode<FastNoise::Simplex> myNoise;
	float myTrauma;
	float myTestTrauma;
	float myShake;
	float myShakeSpeed;
	float myDecayInSeconds;
	float myShakeTimer;

	float myFadeParameter;
	float myFadeSpeed;
	
	ECameraState myState;
	bool myFadingPlaneActive;
	bool myFadePermanent;

	float myFoV; 
};

