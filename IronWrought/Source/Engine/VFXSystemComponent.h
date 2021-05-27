#pragma once
#include "Behavior.h"

class CGameObject;

struct SVFXEffect;

class CVFXSystemComponent : public CBehavior
{
public:
	CVFXSystemComponent(CGameObject& aParent, const std::string& aVFXDataPath);
	~CVFXSystemComponent() override;

	void Init(const std::string& aVFXDataPath);

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	void OnEnable() override;
	void OnDisable() override;

	void EnableEffect(const std::string& anEffectName);
	void DisableEffect(const std::string& anEffectName);
	void EnableEffect(const unsigned int anIndex);
	void DisableEffect(const unsigned int anIndex);

public:
	std::vector<std::unique_ptr<SVFXEffect>>& GetVFXEffects() { return myEffects; }

private:
	std::vector<std::unique_ptr<SVFXEffect>> myEffects;
};

