#pragma once
#include "ResponseBehavior.h"

class CAudioChannel;
namespace PostMaster {
	struct SAudioSourceInitData;
	struct SPlayDynamicAudioData;
}

class CAudioResponse :  public IResponseBehavior
{
public:
	CAudioResponse(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D);
	~CAudioResponse() override;

	void Start() override;
	void Update() override;
	void OnRespond() override;
	void OnDisable() override;

private:
	PostMaster::SAudioSourceInitData mySettings;
	PostMaster::SPlayDynamicAudioData my3DPlayMessage;
	CAudioChannel* myAudioChannel;
	int my2DPlayMessage;
	float myTime;
	bool myIs3D;
};

