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
	CAudioResponse(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings);
	~CAudioResponse() override;

	void Start() override;
	void Update() override;
	void OnRespond() override;

private:
	PostMaster::SAudioSourceInitData mySettings;
	PostMaster::SPlayDynamicAudioData myPlayMessage;
	CAudioChannel* myAudioChannel;
};

