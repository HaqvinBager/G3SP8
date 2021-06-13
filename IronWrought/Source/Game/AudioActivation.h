#pragma once
#include "ActivationBehavior.h"

class CAudioChannel;
namespace PostMaster {
	struct SAudioSourceInitData;
	struct SPlayDynamicAudioData;
}

class CAudioActivation : public IActivationBehavior
{
public:
	CAudioActivation(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D);
	~CAudioActivation() override;

	void Start() override;
	void Update() override;
	void OnActivation() override;
	void OnDisable() override;

private:
	PostMaster::SAudioSourceInitData mySettings;
	PostMaster::SPlayDynamicAudioData my3DPlayMessage;
	CAudioChannel* myAudioChannel;
	int my2DPlayMessage;
	float myTime;
	bool myIs3D;
};
