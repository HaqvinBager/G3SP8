#pragma once
#include <string>

class CFMod;
class CAudio;
class CAudioChannel;

class CFModWrapper
{
public:
	CFModWrapper();
	~CFModWrapper();

	CAudio* RequestSound(const std::string& aFilePath, bool aShouldLoop = false);
	CAudio* Request3DSound(const std::string& aFilePath, bool aShouldLoop = false);
	CAudio* TryGetSound(const std::string& aFilePath, bool aShouldLoop = false);

	CAudioChannel* RequestChannel(const std::string& aChannelName);
	CAudioChannel* RequestAudioSource(const std::string& aSourceIdentifier);

	void Update();

	void Play(CAudio* aSound, CAudioChannel* aChannel);
	void SetListenerAttributes(int aListenerIndex, const Vector3& aListenerPosition, const Vector3& aListenerVelocity, const Vector3& aListenerForward, const Vector3& aListenerUp);
	
private:
	CFMod* myFModInstance;
};

