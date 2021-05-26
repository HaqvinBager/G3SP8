#pragma once
#include "Component.h"

class CGameObject;

class CPhysicsPropAudioComponent : public CComponent
{
public:
	CPhysicsPropAudioComponent(CGameObject& aParent, unsigned int aSoundIndex);
	~CPhysicsPropAudioComponent() override;
	void Awake() override;
	void Start() override;
	void Update() override;

	bool Ready();
	unsigned int& GetSoundIndex();
private:
	unsigned int mySoundIndex;
	float myTimer;
};

