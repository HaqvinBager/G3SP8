#include "stdafx.h"
#include "AudioResponse.h"
#include "AudioChannel.h"
#include "TransformComponent.h"

CAudioResponse::CAudioResponse(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
	, my2DPlayMessage(0)
	, myTime(0.0f)
	, myIs3D(aShouldBe3D)
	, myAudioChannel(nullptr)
{
	if (mySettings.myDelay < 0.001f)
		mySettings.myDelay = 0.001f;

	if (myIs3D)
	{
		myAudioChannel = CEngine::GetInstance()->RequestAudioSource(mySettings);
		my3DPlayMessage.myChannel = myAudioChannel;
		my3DPlayMessage.mySoundIndex = mySettings.mySoundIndex;
	}
	else
	{
		my3DPlayMessage.myChannel = nullptr;
		my3DPlayMessage.mySoundIndex = mySettings.mySoundIndex;
		my2DPlayMessage = mySettings.mySoundIndex;
	}
}

CAudioResponse::~CAudioResponse()
{
	myAudioChannel = nullptr;
	my3DPlayMessage.myChannel = nullptr;
}

void CAudioResponse::Start()
{

}

void CAudioResponse::Update()
{
	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		if (mySettings.myDelay >= myTime)
			return;

		ToggleHasBeenDelayed();
		myTime -= mySettings.myDelay;
	}

	if (myIs3D)
	{
		const Matrix& matrix = GameObject().myTransform->GetLocalMatrix();
		myAudioChannel->Set3DAttributes(matrix.Translation());
		myAudioChannel->Set3DConeAttributes(matrix.Forward(), mySettings.myStartAttenuationAngle, mySettings.myMaxAttenuationAngle, mySettings.myMinimumVolume);
	}
}

void CAudioResponse::OnRespond()
{
	std::cout << __FUNCTION__ << "Play Audio" << std::endl;
	
	if (myIs3D)
		CMainSingleton::PostMaster().SendLate({ EMessageType::PlayDynamicAudioSource, &my3DPlayMessage });
	else
		CMainSingleton::PostMaster().SendLate({ EMessageType::PlaySFX, &my2DPlayMessage });
}

void CAudioResponse::OnDisable()
{
	
}