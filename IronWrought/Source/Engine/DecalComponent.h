#pragma once
#include "Behavior.h"

class CGameObject;
class CDecal;

class CDecalComponent : public CBehavior
{
public:
	struct SSettings {
		std::string myFBXPath;
		int myShouldRenderAlbedo;
		int myShouldRenderMaterial;
		int myShouldRenderNormals;
	};
	CDecalComponent(CGameObject& aParent, const SSettings& someSettings);
	~CDecalComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	CDecal* GetMyDecal() const;
	void SetAlphaThreshold(float aThreshold);

	void OnEnable() override;
	void OnDisable() override;

private:
	CDecal* myDecal;
	SSettings mySettings;
	//std::string myDecalPath;
};

