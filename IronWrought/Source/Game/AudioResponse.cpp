#include "stdafx.h"
#include "AudioResponse.h"
#include "AudioChannel.h"
#include "TransformComponent.h"

CAudioResponse::CAudioResponse(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
{
	myAudioChannel = CEngine::GetInstance()->RequestAudioSource(mySettings);
	myPlayMessage.myChannel = myAudioChannel;
	myPlayMessage.mySoundIndex = mySettings.mySoundIndex;
}

CAudioResponse::~CAudioResponse()
{

}

void CAudioResponse::Start()
{

}

void CAudioResponse::Update()
{
	const Matrix& matrix = GameObject().myTransform->GetWorldMatrix();
	myAudioChannel->Set3DAttributes(matrix.Translation());
	myAudioChannel->Set3DConeAttributes(matrix.Forward(), mySettings.myStartAttenuationAngle, mySettings.myMaxAttenuationAngle, mySettings.myMinimumVolume);
}

void CAudioResponse::OnRespond()
{
	std::cout << __FUNCTION__ << "Play Audio" << std::endl;
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayDynamicAudioSource, &myPlayMessage });
}

void CAudioResponse::OnDisable()
{
	
}