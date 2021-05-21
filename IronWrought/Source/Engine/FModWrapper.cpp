#include "stdafx.h"
#include "FModWrapper.h"
#include "FMod.h"
#include "Audio.h"
#include "AudioChannel.h"

CFModWrapper::CFModWrapper()
{
	myFModInstance = new CFMod();
}

CFModWrapper::~CFModWrapper()
{
	delete myFModInstance;
	myFModInstance = nullptr;
}

CAudio* CFModWrapper::RequestSound(const std::string& aFilePath, bool aShouldLoop)
{
	CAudio* sound = new CAudio(myFModInstance->CreateSound(aFilePath, aShouldLoop));
	return std::move(sound);
}

CAudio* CFModWrapper::Request3DSound(const std::string& aFilePath, bool aShouldLoop)
{
	CAudio* sound = new CAudio(myFModInstance->CreateSound(aFilePath, aShouldLoop, true));
	return std::move(sound);
}

CAudio* CFModWrapper::TryGetSound(const std::string& aFilePath, bool aShouldLoop)
{
	FMOD::Sound* sound = myFModInstance->TryCreateSound(aFilePath, aShouldLoop);
	
	if (sound)
	{
		CAudio* audio = new CAudio(sound);
		return std::move(audio);
	}

	return nullptr;
}

CAudioChannel* CFModWrapper::RequestChannel(const std::string& aChannelName)
{
	CAudioChannel* channel = new CAudioChannel(myFModInstance->CreateChannel(aChannelName));
	return std::move(channel);
}

CAudioChannel* CFModWrapper::RequestAudioSource(const std::string& aSourceIdentifier)
{
	CAudioChannel* channel = new CAudioChannel(myFModInstance->CreateChannel(aSourceIdentifier, true));
	return std::move(channel);
}

void CFModWrapper::Update()
{
	myFModInstance->Update();
}

void CFModWrapper::Play(CAudio* aSound, CAudioChannel* aChannel)
{
	myFModInstance->Play(aSound->myFModSound, aChannel->myFModChannel);
}

void CFModWrapper::SetListenerAttributes(int aListenerIndex, const Vector3& aListenerPosition, const Vector3& aListenerVelocity, const Vector3& aListenerForward, const Vector3& aListenerUp)
{
	myFModInstance->SetListenerAttributes
	(
		aListenerIndex
		, { aListenerPosition.x, aListenerPosition.y, aListenerPosition.z }
		, { aListenerVelocity.x, aListenerVelocity.y, aListenerVelocity.z }
		, { aListenerForward.x, aListenerForward.y, aListenerForward.z }
		, { aListenerUp.x, aListenerUp.y, aListenerUp.z }
	);
}
