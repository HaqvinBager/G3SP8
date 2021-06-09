#pragma once
#include "Component.h"

class CAudioChannel;
class CGameObject;

namespace PostMaster {
	struct SAudioSourceInitData;
	struct SPlayDynamicAudioData;
}

class CPhysicsPropAudioComponent : public CComponent
{
public:
	CPhysicsPropAudioComponent(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings);
	~CPhysicsPropAudioComponent() override;
	void Awake() override;
	void Start() override;
	void Update() override;

	bool Ready();
	PostMaster::SPlayDynamicAudioData& GetPlayMessage();
private:
	PostMaster::SAudioSourceInitData mySettings;
	PostMaster::SPlayDynamicAudioData myPlayMessage;
	CAudioChannel* myAudioChannel;
	float myTimer;
};

