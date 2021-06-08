#pragma once
#include "ResponseBehavior.h"

class CAudioChannel;
namespace PostMaster {
	struct SAudioSourceInitData;
	struct SPlayDynamicAudioData;
}

class CVoiceResponse : public IResponseBehavior
{
public:
	CVoiceResponse(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D);
	~CVoiceResponse() override;

	void Start() override;
	void Update() override;
	void OnRespond() override;
	void OnDisable() override;

private:
	PostMaster::SAudioSourceInitData mySettings;
	PostMaster::SPlayDynamicAudioData my3DPlayMessage;
	CAudioChannel* myAudioChannel;
	int my2DPlayMessage;
	bool myIs3D;
};

