#pragma once

namespace FMOD {
	class ChannelGroup;
}

class CAudioChannel
{
	friend class CFModWrapper;
public:
	const bool IsPlaying() const;

	const int PlayCount() const;

	void SetVolume(float aVolumeLevel);
	void Stop();
	void Mute(bool aDoMute);
	void SetPaused(bool aDoPause);
	void SetPitch(float aPitch);

	//Update
	void Set3DAttributes(const Vector3& aPosition, const Vector3& aVelocity = { 0.0f, 0.0f, 0.0f });
	void Set3DConeAttributes(const Vector3& aDirection, float anUnattenuatedAngleSpread, float anAttenuatedAngleSpread, float anAttenuatedVolume);
	void Set3DMinMaxDistance(float aMinDistance, float aMaxDistance);

	void SetIs3D(bool aShouldBe3D);

public:
	~CAudioChannel();// 2020 12 04  - Destruction of CAudioChannel is not taken care of by FMOD
private:
	// CAudioChannel should be created from CFModWrapper
	CAudioChannel(FMOD::ChannelGroup* aChannelPointer);

private:
	FMOD::ChannelGroup* myFModChannel;
};

