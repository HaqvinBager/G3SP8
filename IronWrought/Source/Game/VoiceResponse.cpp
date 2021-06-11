#include "stdafx.h"
#include "VoiceResponse.h"
#include "AudioChannel.h"
#include "TransformComponent.h"

CVoiceResponse::CVoiceResponse(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
	, my2DPlayMessage(0)
	, myIs3D(aShouldBe3D)
{
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

CVoiceResponse::~CVoiceResponse()
{
}

void CVoiceResponse::Start()
{
}

void CVoiceResponse::Update()
{
	if (myIs3D)
	{
		const Matrix& matrix = GameObject().myTransform->GetLocalMatrix();
		Vector3 pos = matrix.Translation();
		myAudioChannel->Set3DAttributes(pos);
		myAudioChannel->Set3DConeAttributes(matrix.Forward(), mySettings.myStartAttenuationAngle, mySettings.myMaxAttenuationAngle, mySettings.myMinimumVolume);
	}
}

void CVoiceResponse::OnRespond()
{
	std::cout << __FUNCTION__ << "Play Voice" << std::endl;
	CMainSingleton::PostMaster().SendLate({ EMessageType::LoadDialogue, &my3DPlayMessage });
	//if (myIs3D)
	//	CMainSingleton::PostMaster().SendLate({ EMessageType::Play3DVoiceLine, &my3DPlayMessage });
	//else
	//	CMainSingleton::PostMaster().SendLate({ EMessageType::Play2DVoiceLine, &my2DPlayMessage });
}

void CVoiceResponse::OnDisable()
{
}
