#include "stdafx.h"
#include "AudioChannel.h"
#include <fmod/fmod.hpp>

const bool CAudioChannel::IsPlaying() const
{
    bool isPlaying = false;
    myFModChannel->isPlaying(&isPlaying);
    return isPlaying;
}

const int CAudioChannel::PlayCount() const
{
    int count = 0;
    myFModChannel->getNumChannels(&count);
    return count;
}

const float CAudioChannel::GetVolume() const
{
    float vol = 0.0f;
    myFModChannel->getVolume(&vol);
    return vol;
}

void CAudioChannel::SetVolume(float aVolumeLevel)
{
    myFModChannel->setVolume(aVolumeLevel);
}

void CAudioChannel::Stop()
{
    myFModChannel->stop();
}

void CAudioChannel::Mute(bool aDoMute)
{
    myFModChannel->setMute(aDoMute);
}

void CAudioChannel::SetPaused(bool aDoPause)
{
    myFModChannel->setPaused(aDoPause);
}

void CAudioChannel::SetPitch(float aPitch)
{
    myFModChannel->setPitch(aPitch);
}

void CAudioChannel::Set3DAttributes(const Vector3& aPosition, const Vector3& aVelocity)
{
    FMOD_VECTOR pos = { aPosition.x, aPosition.y, aPosition.z };
    FMOD_VECTOR vel = { aVelocity.x, aVelocity.y, aVelocity.z };
    myFModChannel->set3DAttributes(&pos, &vel);
}

void CAudioChannel::Set3DConeAttributes(const Vector3& aDirection, float anUnattenuatedAngleSpread, float anAttenuatedAngleSpread, float anAttenuatedVolume)
{
    FMOD_VECTOR orientation = { aDirection.x, aDirection.y, aDirection.z };
    myFModChannel->set3DConeOrientation(&orientation);
    myFModChannel->set3DConeSettings(anUnattenuatedAngleSpread, anAttenuatedAngleSpread, anAttenuatedVolume);
}

void CAudioChannel::Set3DMinMaxDistance(float aMinDistance, float aMaxDistance)
{
    myFModChannel->set3DMinMaxDistance(aMinDistance, aMaxDistance);
}

void CAudioChannel::SetIs3D(bool aShouldBe3D)
{
    if (aShouldBe3D)
        myFModChannel->setMode(FMOD_3D);
    else
        myFModChannel->setMode(FMOD_2D);
}

CAudioChannel::CAudioChannel(FMOD::ChannelGroup* aChannelPointer)
{
    myFModChannel = aChannelPointer;
}

CAudioChannel::~CAudioChannel()
{
    if (myFModChannel)
        myFModChannel->release();
    //delete myFModChannel;
    myFModChannel = nullptr;
}
