#include "stdafx.h"
#include "AudioActivation.h"
#include "AudioChannel.h"
#include "TransformComponent.h"
#include "RotateActivation.h"

CAudioActivation::CAudioActivation(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings, bool aShouldBe3D)
	: IActivationBehavior(aParent)
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

CAudioActivation::~CAudioActivation()
{
	myAudioChannel = nullptr;
	my3DPlayMessage.myChannel = nullptr;
}

void CAudioActivation::Start()
{
}

void CAudioActivation::Update()
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
		//This AudioActivation's Specific AudioClip is Toggled Between soundeIndex 12 and 35
		//This switch is dependant on when a Rotator Component (attached to the same GameObject)
		//has fully completed its rotate activation.
		CRotateActivation* rotator = nullptr;
		if (GameObject().TryGetComponent(&rotator)) 
		{
			if (Complete(rotator->Complete()))
			{
				if (!ToggleAudioClip())
				{
					Complete(!myAudioChannel->IsPlaying());
				}
			}
		}
		else
		{
			Complete(!myAudioChannel->IsPlaying());
		}
	}

	if (myIsInteracted && myIs3D)
	{
		const Matrix& matrix = GameObject().myTransform->GetWorldMatrix();
		myAudioChannel->Set3DAttributes(matrix.Translation());
		myAudioChannel->Set3DConeAttributes(matrix.Forward(), mySettings.myStartAttenuationAngle, mySettings.myMaxAttenuationAngle, mySettings.myMinimumVolume);
	}
}

bool CAudioActivation::ToggleAudioClip()
{
	//std::cout << "Toggle Audio Bool" << std::endl;
	// Open
	if (my3DPlayMessage.mySoundIndex == 35)
	{
		my3DPlayMessage.mySoundIndex = 12;
		my2DPlayMessage = 12;
		return true;
	}
	// Close
	else if (my3DPlayMessage.mySoundIndex == 12)
	{
		my3DPlayMessage.mySoundIndex = 35;
		my2DPlayMessage = 35;
		return true;
	}

	return false;
}

const int CAudioActivation::GetAudioIndex() const
{
	return mySettings.mySoundIndex;
}

void CAudioActivation::OnActivation()
{
	//std::cout << __FUNCTION__ << "Play Audio" << std::endl;
	myIsInteracted = true;

	if (myIs3D)
		CMainSingleton::PostMaster().Send({ EMessageType::PlayDynamicAudioSource, &my3DPlayMessage });
	else
		CMainSingleton::PostMaster().Send({EMessageType::PlaySFX, &my2DPlayMessage});
}

void CAudioActivation::OnDisable()
{
}

