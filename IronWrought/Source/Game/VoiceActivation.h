#pragma once
#include "ActivationBehavior.h"

class CAudioChannel;
namespace PostMaster {
	struct SAudioSourceInitData;
	struct SPlayDynamicAudioData;
}

class CVoiceActivation : public IActivationBehavior
{
public:
	CVoiceActivation(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D);
	~CVoiceActivation() override;

	void Start() override;
	void Update() override;
	void OnActivation() override;
	void OnDisable() override;

private:
	PostMaster::SAudioSourceInitData mySettings;
	PostMaster::SPlayDynamicAudioData my3DPlayMessage;
	CAudioChannel* myAudioChannel;
	int my2DPlayMessage;
	bool myIs3D;
};

