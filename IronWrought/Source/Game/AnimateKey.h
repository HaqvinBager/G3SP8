#pragma once
#include "KeyBehavior.h"

class CAnimateKey : public CKeyBehavior
{
public:
	struct SAnimateKeySettings
	{
		Vector3 myStartPosition;
		Vector3 myEndPosition;

		Vector3 myStartRotation;
		Vector3 myEndRotation;
		
		float myDuration;
	};

	CAnimateKey(CGameObject& aParent, const SSettings& someSettings, const SAnimateKeySettings& someAnimateKeySettings);
	~CAnimateKey() override;
	void OnInteract() override;
	void Update() override;
private:
	SAnimateKeySettings myAnimateKeySettings;
	bool myIsInteracted;
	float myTime;
};