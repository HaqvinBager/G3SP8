#include "stdafx.h"
#include "AudioActivation.h"
#include "AudioChannel.h"
#include "TransformComponent.h"

CAudioActivation::CAudioActivation(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
{
	myAudioChannel = CEngine::GetInstance()->RequestAudioSource(mySettings);
	myPlayMessage.myChannel = myAudioChannel;
	myPlayMessage.mySoundIndex = mySettings.mySoundIndex;
}

CAudioActivation::~CAudioActivation()
{
}

void CAudioActivation::Start()
{
}

void CAudioActivation::Update()
{
	if (myIsInteracted)
	{
		const Matrix& matrix = GameObject().myTransform->GetWorldMatrix();
		myAudioChannel->Set3DAttributes(matrix.Translation());
		myAudioChannel->Set3DConeAttributes(matrix.Forward(), mySettings.myStartAttenuationAngle, mySettings.myMaxAttenuationAngle, mySettings.myMinimumVolume);
		Complete(!myAudioChannel->IsPlaying());
	}
}

void CAudioActivation::OnActivation()
{
	std::cout << __FUNCTION__ << "Play Audio" << std::endl;
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayDynamicAudioSource, &myPlayMessage });
}

void CAudioActivation::OnDisable()
{
}
