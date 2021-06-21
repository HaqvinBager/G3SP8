#pragma once
#include "ResponseBehavior.h"
#include "FastNoise\FastNoise.h"

class CSpotLightComponent;
class CPointLightComponent;
class CBehavior;

class CLightFlickerResponse : public IResponseBehavior
{
public:
	struct SSettings {
		float mySpeed;
		Vector2 myIntensity;
		//Color myColorA;
		//Color myColorB;
	};

	CLightFlickerResponse(CGameObject& aParent, const SSettings& someSettings);
	~CLightFlickerResponse() override;

	void Awake() override;
	void Update() override;
	void OnRespond() override;

	float Noise() const;

private:

	inline float InverseLerp(float a, float b, float t) const
	{
		return (t - a) / (b - a);
	}

	inline float Lerp(float a, float b, float t) const
	{
		return (1.0f - t) * a + b * t;
	}

	inline float Remap(const float iMin, const float iMax, const float oMin, const float oMax, const float v) const
	{
		float t = InverseLerp(iMin, iMax, v);
		return Lerp(oMin, oMax, t);
	}

	void UpdateNoise();

	SSettings mySettings;
	int myLightType;
	float myIntensity;
	CBehavior* myLight;
	FastNoise::SmartNode<FastNoise::Simplex> myNoise;
};

