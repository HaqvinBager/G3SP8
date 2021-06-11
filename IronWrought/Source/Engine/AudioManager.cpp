#include "stdafx.h"
#include "AudioManager.h"
#include "Audio.h"
#include "AudioChannel.h"
#include "MainSingleton.h"
#include "PostMaster.h"
#include "JsonReader.h"
#include "RandomNumberGenerator.h"
#include "GameObject.h"

using namespace rapidjson;

#define CAST(type) { static_cast<unsigned int>(type) }
#define AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION 1

CAudioManager::CAudioManager()
	: myWrapper()
	, myCurrentGroundType(EGroundType::Concrete)
	, myDynamicChannel1(0.0f)
	, myDynamicChannel2(0.0f)
	, myDynamicChannel3(0.0f)
	, myListener(nullptr)
	, myDynamicObject(nullptr)
	, myDynamicSource(nullptr)
{
	SubscribeToMessages();

	// Init Channels
	for (unsigned int i = 0; i < static_cast<unsigned int>(EChannel::Count); ++i) {
		myChannels.push_back(myWrapper.RequestChannel(TranslateEnum(static_cast<EChannel>(i))));
	}

	// Init Sounds
	for (unsigned int i = 0; i < static_cast<unsigned int>(EMusic::Count); ++i)
	{
		myMusicAudio.push_back(myWrapper.RequestSound(GetPath(static_cast<EMusic>(i)), true));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(EAmbience::Count); ++i)
	{
		myAmbienceAudio.push_back(myWrapper.RequestSound(GetPath(static_cast<EAmbience>(i)), true));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(EPropAmbience::Count); ++i)
	{
		myPropAmbienceAudio.push_back(myWrapper.RequestSound(GetPath(static_cast<EPropAmbience>(i)), true));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(ESFX::Count); ++i)
	{
		mySFXAudio.push_back(myWrapper.RequestSound(GetPath(static_cast<ESFX>(i))));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(ESFXCollection::Count); ++i)
	{
		FillCollection(static_cast<ESFXCollection>(i));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(EUI::Count); ++i)
	{
		myUIAudio.push_back(myWrapper.RequestSound(GetPath(static_cast<EUI>(i))));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(EVOX::Count); ++i)
	{
		myVoiceEventSounds.push_back(myWrapper.RequestSound(GetPath(static_cast<EVOX>(i))));
	}

	LoadEnemyLines();

	std::ifstream volumeStream("Json/Audio/AudioVolume.json");
	IStreamWrapper volumeWrapper(volumeStream);
	Document volDoc;
	volDoc.ParseStream(volumeWrapper);

	if (volDoc.HasParseError()) { return; }

	if (volDoc.HasMember("Ambience"))
	{
		float value = volDoc["Ambience"].GetFloat();
		myChannels[CAST(EChannel::Ambience)]->SetVolume(value);
	}
	if (volDoc.HasMember("Music"))
	{
		float value = volDoc["Music"].GetFloat();
		myChannels[CAST(EChannel::Music)]->SetVolume(value);
	}
	if (volDoc.HasMember("SFX"))
	{
		float value = volDoc["SFX"].GetFloat();
		myChannels[CAST(EChannel::SFX)]->SetVolume(value);
	}
	if (volDoc.HasMember("UI"))
	{
		float value = volDoc["UI"].GetFloat();
		myChannels[CAST(EChannel::UI)]->SetVolume(value);
	}
	if (volDoc.HasMember("ResearcherVoice"))
	{
		float value = volDoc["ResearcherVoice"].GetFloat();
		myChannels[CAST(EChannel::VOX)]->SetVolume(value);
	}

	myDynamicSource = myWrapper.RequestAudioSource("Enemy");
	myDynamicSource->Set3DMinMaxDistance(10.0f, 1000.0f);
	//myDynamicSource->SetVolume(0.2f);

	SetDynamicTrack(EAmbience::Cottage, EAmbience::Basement1, EAmbience::Basement2);

	myChannels[CAST(EChannel::DynamicChannel1)]->SetVolume(0.0f);
	myChannels[CAST(EChannel::DynamicChannel2)]->SetVolume(0.0f);
	myChannels[CAST(EChannel::DynamicChannel3)]->SetVolume(0.0f);
}

CAudioManager::~CAudioManager()
{
	UnsubscribeToMessages();
	// 2021 03 08 After using std::move in Request methods sounds seem to be destroyable. Channels still can't be safely destroyed though
	//for (auto& channel : myChannels)
	//{
	//	delete channel;
	//	channel = nullptr;
	//}
	for (auto& music : myMusicAudio)
	{
		delete music;
		music = nullptr;
	}
	for (auto& ambience : myAmbienceAudio)
	{
		delete ambience;
		ambience = nullptr;
	}
	for (auto& sfx : mySFXAudio)
	{
		delete sfx;
		sfx = nullptr;
	}
	for (auto& ui : myUIAudio)
	{
		delete ui;
		ui = nullptr;
	}
	for (auto& sound : myWoodStepSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myWoodFastStepSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myCarpetStepSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myCarpetFastStepSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myConcreteStepSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myConcreteFastStepSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myVoiceEventSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myRobotAttackSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myRobotDeathSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myRobotIdleSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myRobotPatrollingSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myRobotSearchingSounds)
	{
		delete sound;
		sound = nullptr;
	}
}

void CAudioManager::Receive(const SMessage& aMessage) {
	switch (aMessage.myMessageType)
	{
		// UI
	case EMessageType::UIButtonPress:
	{
		if (myUIAudio.size() >= static_cast<unsigned int>(EUI::ButtonClick))
		{
			myWrapper.Play(myUIAudio[CAST(EUI::ButtonClick)], myChannels[CAST(EChannel::UI)]);
		}
	}break;

	case EMessageType::PlayStepSound:
	{
		PostMaster::SStepSoundData stepData = *reinterpret_cast<PostMaster::SStepSoundData*>(aMessage.data);
		int groundType = stepData.myGroundMaterial/**reinterpret_cast<int*>(aMessage.data)*/;

		if (groundType == 0 || groundType == 1/*myCurrentGroundType == EGroundType::Concrete*/)
		{
			if (stepData.myIsSprint)
				PlayCyclicRandomSoundFromCollection(myWoodFastStepSounds, EChannel::SFX, myStepSoundIndices, 1);
			else
				PlayCyclicRandomSoundFromCollection(myWoodStepSounds, EChannel::SFX, myStepSoundIndices, 1);

		}
		else if (groundType == 2/*myCurrentGroundType == EGroundType::AirVent*/)
		{
			if (stepData.myIsSprint)
				PlayCyclicRandomSoundFromCollection(myCarpetFastStepSounds, EChannel::SFX, myStepSoundIndices, 1);
			else
				PlayCyclicRandomSoundFromCollection(myCarpetStepSounds, EChannel::SFX, myStepSoundIndices, 1);
		}
		else if (groundType == 3/*myCurrentGroundType == EGroundType::AirVent*/)
		{
			if (stepData.myIsSprint)
				PlayCyclicRandomSoundFromCollection(myConcreteFastStepSounds, EChannel::SFX, myStepSoundIndices, 1);
			else 
				PlayCyclicRandomSoundFromCollection(myConcreteStepSounds, EChannel::SFX, myStepSoundIndices, 1);

		}
	}
	break;

	case EMessageType::PlayerTakeDamage:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::EnemyHit)], myChannels[CAST(EChannel::SFX)]);
	}
	break;

	case EMessageType::PlayVoiceEvent:
	{
		int index = *static_cast<int*>(aMessage.data);
		myChannels[CAST(EChannel::VOX)]->Stop();
		myWrapper.Play(myVoiceEventSounds[index], myChannels[CAST(EChannel::VOX)]);
	
		CMainSingleton::PostMaster().Send({ EMessageType::LoadDialogue, &index });
	}
	break;

	case EMessageType::PlaySFX:
	{
		int index = *static_cast<int*>(aMessage.data);
		myWrapper.Play(mySFXAudio[index], myChannels[CAST(EChannel::SFX)]);
	}
	break;

	case EMessageType::GravityGlovePull:
	{
		bool release = false;
		if (aMessage.data)
			release = *static_cast<bool*>(aMessage.data);
		if (release)
			myWrapper.Play(mySFXAudio[CAST(ESFX::LetGo)], myChannels[CAST(EChannel::SFX)]);
		else
			myWrapper.Play(mySFXAudio[CAST(ESFX::Grab)], myChannels[CAST(EChannel::SFX)]);
	}break;

	case EMessageType::GravityGlovePush:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::Throw)], myChannels[CAST(EChannel::SFX)]);
	}break;

	case EMessageType::PlayerHealthPickup:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::PickupHeal)], myChannels[CAST(EChannel::SFX)]);
	}break;


	case EMessageType::EnemyAttackState:
	{
		if (myChannels[CAST(EChannel::VOX)]->IsPlaying())
			return;

	}break;

	case EMessageType::EnemyPatrolState:
	{
		myDynamicSource->Stop();
		myWrapper.Play(myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyPatrol)], myDynamicSource);
	}break;

	case EMessageType::EnemySeekState:
	{
		//myDynamicSource->Stop();
		//std::cout << "Play Seek" << std::endl;
		//myWrapper.Play(myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyChasing)], myDynamicSource);
	}break;

	case EMessageType::EnemyAlertedState:
	{
		myDynamicSource->Stop();
		myWrapper.Play(myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyHeardNoise)], myDynamicSource);
	}break;

	case EMessageType::EnemyIdleState:
	{
		myDynamicSource->Stop();
		myWrapper.Play(myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyLostPlayer)], myDynamicSource);
	}break;

	case EMessageType::EnemyFoundPlayer:
	{
		myDynamicSource->Stop();
		myWrapper.Play(myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyFoundPlayer)], myDynamicSource);
		myDelayedAudio.push_back({myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyChasing)], myDynamicSource, 4.0f});
		FadeChannelOverSeconds(EChannel::DynamicChannel2, 4.0f);
		FadeChannelOverSeconds(EChannel::DynamicChannel3, 4.0f, false);
	}break;

	case EMessageType::EnemyLostPlayer:
	{
		myDynamicSource->Stop();
		myWrapper.Play(myEnemyVoiceSounds[CAST(EEnemyVoiceLine::EnemyLostPlayer)], myDynamicSource);
		FadeChannelOverSeconds(EChannel::DynamicChannel2, 4.0f, false);
		FadeChannelOverSeconds(EChannel::DynamicChannel3, 4.0f);
	}break;

	case EMessageType::EnemyAttack:
	{
		if (myChannels[CAST(EChannel::VOX)]->IsPlaying())
			return;

		if (mySFXAudio[CAST(ESFX::EnemyAttack)])
			myWrapper.Play(mySFXAudio[CAST(ESFX::EnemyAttack)], myChannels[CAST(EChannel::SFX)]);
	}break;

	case EMessageType::GameStarted:
	{
		myWrapper.Play(myVoiceEventSounds[CAST(EVOX::Line1)], myChannels[CAST(EChannel::VOX)]);

	}break;

	case EMessageType::StartGame:
	{
		//std::string scene = *reinterpret_cast<std::string*>(aMessage.data);
		//if (strcmp(scene.c_str(), "VerticalSlice") == 0)
		//{
		//	myChannels[CAST(EChannel::Ambience)]->Stop();
		//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		//	return;
		//}

		//if (strcmp(scene.c_str(), "Level_1-1") == 0)
		//{
		//	myChannels[CAST(EChannel::Ambience)]->Stop();
		//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		//	return;
		//}

		//if (strcmp(scene.c_str(), "Level_1-2") == 0)
		//{
		//	myChannels[CAST(EChannel::Ambience)]->Stop();
		//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		//	return;
		//}

		//if (strcmp(scene.c_str(), "Level_2-1") == 0)
		//{
		//	myChannels[CAST(EChannel::Ambience)]->Stop();
		//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
		//	return;
		//}

		//if (strcmp(scene.c_str(), "Level_2-2") == 0)
		//{
		//	myChannels[CAST(EChannel::Ambience)]->Stop();
		//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
		//	return;
		//}
	}break;

	case EMessageType::BootUpState:
	{
		//myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		myWrapper.Play(myVoiceEventSounds[CAST(EVOX::Line0)], myChannels[CAST(EChannel::VOX)]);
	}break;

	case EMessageType::MainMenu:
	{
		myChannels[CAST(EChannel::Ambience)]->Stop();
		myChannels[CAST(EChannel::Music)]->Stop();
		myChannels[CAST(EChannel::VOX)]->Stop();
		myChannels[CAST(EChannel::SFX)]->Stop();
		//myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
	}break;

	case EMessageType::AddStaticAudioSource:
	{
		PostMaster::SAudioSourceInitData data = *reinterpret_cast<PostMaster::SAudioSourceInitData*>(aMessage.data);

		if (data.mySoundIndex < 0 || data.mySoundIndex >= static_cast<int>(EPropAmbience::Count))
			return;

		this->AddStaticSource(data);
	}break;

	case EMessageType::SetDynamicAudioSource:
	{
		CGameObject* data = reinterpret_cast<CGameObject*>(aMessage.data);
		myDynamicObject = data;
		myDynamicObjects.push_back(data);
	}break;

	case EMessageType::PhysicsPropCollision:
	{
		PostMaster::SPlayDynamicAudioData data = *static_cast<PostMaster::SPlayDynamicAudioData*>(aMessage.data);
		data.myChannel->SetPitch(Random(0.95f, 1.05f));
		myWrapper.Play(mySFXAudio[data.mySoundIndex], data.myChannel);
	}break;

	case EMessageType::PlayDynamicAudioSource:
	{
		PostMaster::SPlayDynamicAudioData data = *static_cast<PostMaster::SPlayDynamicAudioData*>(aMessage.data);
		myWrapper.Play(mySFXAudio[data.mySoundIndex], data.myChannel);
	}break;
	
	case EMessageType::Play3DVoiceLine:
	{
		PostMaster::SPlayDynamicAudioData data = *static_cast<PostMaster::SPlayDynamicAudioData*>(aMessage.data);
		myWrapper.Play(myVoiceEventSounds[data.mySoundIndex], data.myChannel);
	}break;

	case EMessageType::Play2DVoiceLine:
	{
		int data = *static_cast<int*>(aMessage.data);
		myWrapper.Play(myVoiceEventSounds[data], myChannels[CAST(EChannel::VOX)]);
	}break;

	case EMessageType::SetAmbience:
	{
		PostMaster::ELevelName level = *reinterpret_cast<PostMaster::ELevelName*>(aMessage.data);
		
		switch (level)
		{
		case PostMaster::ELevelName::Cottage_1:
		{
			FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f, false);
			FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f);
			FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		}break;
		case PostMaster::ELevelName::Cottage_2:
		{
			FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f, false);
			FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f);
			FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		}break;
		//case PostMaster::ELevelName::Basement_1_1_A:
		//{
		//	FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f, false);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		//}break;
		//case PostMaster::ELevelName::Basement_1_1_B:
		//{
		//	FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f, false);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		//}break;
		//case PostMaster::ELevelName::Basement_1_2_A:
		//{
		//	FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f, false);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		//}break;
		//case PostMaster::ELevelName::Basement_1_2_B:
		//{
		//	FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f, false);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f);
		//	FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		//}break;
		case PostMaster::ELevelName::Basement_1_3:
		{
			FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f, false);
			FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f);
			FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		}break;
		case PostMaster::ELevelName::Basement_2:
		{
			FadeChannelOverSeconds(EChannel::DynamicChannel2, 1.0f, false);
			FadeChannelOverSeconds(EChannel::DynamicChannel1, 1.0f);
			FadeChannelOverSeconds(EChannel::DynamicChannel3, 1.0f);
		}break;
		default:
			break;
		}
	}break;
	
	case EMessageType::PauseMenu:
	{
		Pause();
	}break;

	case EMessageType::Resume:
	{
		Resume();
	}break;

	default: break;
	}
}

void CAudioManager::Receive(const SStringMessage& /*aMessage*/)
{
	//Puzzle Clips
	//

	//myWrapper.Play(myAmbienceAudio[0], myChannels[0]);


	//if (strcmp(aMessage.myMessageType, "Level_1-1") == 0)
	//{
	//	myChannels[CAST(EChannel::Ambience)]->Stop();
	//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
	//	return;
	//}

	//if (strcmp(aMessage.myMessageType, "Level_1-2") == 0)
	//{
	//	myChannels[CAST(EChannel::Ambience)]->Stop();
	//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
	//	return;
	//}

	//if (strcmp(aMessage.myMessageType, "Level_2-1") == 0)
	//{
	//	myChannels[CAST(EChannel::Ambience)]->Stop();
	//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
	//	return;
	//}

	//if (strcmp(aMessage.myMessageType, "Level_2-2") == 0)
	//{
	//	myChannels[CAST(EChannel::Ambience)]->Stop();
	//	myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
	//	return;
	//}

}

void CAudioManager::Update()
{
	if (myListener)
	{
		myWrapper.SetListenerAttributes(0, myListener->myTransform->WorldPosition(), { 0.0f, 0.0f, 0.0f }, myListener->myTransform->GetWorldMatrix().Forward(), myListener->myTransform->GetWorldMatrix().Up());
	}

	if (myDynamicObject)
	{
		myDynamicSource->Set3DAttributes(myDynamicObject->myTransform->WorldPosition(), { 0.0f, 0.0f, 0.0f });
		myDynamicSource->Set3DConeAttributes(myDynamicObject->myTransform->Transform().Forward(), 90.0f, 300.0f, 0.3f);
	}

	myWrapper.Update();

	if (myDelayedAudio.size() > 0)
	{
		const float dt = CTimer::Dt();

		for (auto it = myDelayedAudio.begin(); it != myDelayedAudio.end();)
		{
			it->myTimer -= dt;
			if (it->myTimer <= 0.0f)
			{
				it->myChannel->Stop();
				myWrapper.Play(it->myAudio, it->myChannel);
				it = myDelayedAudio.erase(it);
				continue;
			}
			++it;
		}
	}

	if (myFadingChannels.size() > 0)
	{
		const float dt = CTimer::Dt();

		for (auto it = myFadingChannels.begin(); it != myFadingChannels.end();)
		{
			it->myTimer -= dt;

			float newVolume = std::clamp(it->myTimer, 0.0f, it->myDuration);
			newVolume /= it->myDuration;
			newVolume = it->myFadeOut ? newVolume : (1.0f - newVolume);

			myChannels[CAST(it->myChannel)]->SetVolume(newVolume);

			if (it->myTimer <= 0.0f)
			{
				it = myFadingChannels.erase(it);
				continue;
			}
			++it;
		}
	}
}

void CAudioManager::SetListener(CGameObject* aGameObject)
{
	myListener = aGameObject;
}

CAudioChannel* CAudioManager::AddSource(const PostMaster::SAudioSourceInitData& someData) 
{
	myDynamicSources.push_back(myWrapper.RequestAudioSource(std::to_string(someData.myGameObjectID)));
	myDynamicSources.back()->Set3DMinMaxDistance(someData.myMinAttenuationDistance, someData.myMaxAttenuationDistance);
	return myDynamicSources.back();
}

void CAudioManager::AddStaticSource(const PostMaster::SAudioSourceInitData& someData)
{
	myStaticAudioSources.push_back({ someData.myGameObjectID, static_cast<unsigned int>(someData.mySoundIndex), myWrapper.RequestAudioSource("3D") });
	myStaticAudioSources.back().myChannel->Set3DAttributes(someData.myPosition, Vector3::Zero);
	myStaticAudioSources.back().myChannel->Set3DConeAttributes(someData.myForward, someData.myStartAttenuationAngle, someData.myMaxAttenuationAngle, someData.myMinimumVolume);
	myStaticAudioSources.back().myChannel->Set3DMinMaxDistance(someData.myMinAttenuationDistance, someData.myMaxAttenuationDistance);
	myWrapper.Play(myPropAmbienceAudio[someData.mySoundIndex], myStaticAudioSources.back().myChannel);
}

void CAudioManager::RemoveSource(const int /*anIdentifier*/)
{
}

void CAudioManager::ClearSources()
{
	myStaticAudioSources.clear();
}

void CAudioManager::SubscribeToMessages()
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::UIButtonPress, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayResearcherReactionExplosives, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayRobotAttackSound, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayRobotDeathSound, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayRobotIdleSound, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayRobotPatrolling, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayRobotSearching, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayVoiceEvent, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlaySFX, this);

	// Player & Gravity Glove 
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayStepSound, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayJumpSound, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerTakeDamage, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::GravityGlovePull, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::GravityGlovePush, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerHealthPickup, this);

	// Enemy
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyPatrolState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemySeekState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttackState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAlertedState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyIdleState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttack, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyTakeDamage, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyFoundPlayer, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyLostPlayer, this);

	//CMainSingleton::PostMaster().Subscribe(EMessageType::PlayVoiceLine, this);
	//CMainSingleton::PostMaster().Subscribe(EMessageType::StopDialogue, this);

	// Ambience
	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Subscribe("Level_1-1", this);
	CMainSingleton::PostMaster().Subscribe("Level_1-2", this);
	CMainSingleton::PostMaster().Subscribe("Level_2-1", this);
	CMainSingleton::PostMaster().Subscribe("Level_2-2", this);

	CMainSingleton::PostMaster().Subscribe(EMessageType::BootUpState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::GameStarted, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PauseMenu, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Resume, this);

	CMainSingleton::PostMaster().Subscribe(EMessageType::AddStaticAudioSource, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::ClearStaticAudioSources, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetDynamicAudioSource, this);

	CMainSingleton::PostMaster().Subscribe(EMessageType::PhysicsPropCollision, this);

	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyStateChange, this);

	CMainSingleton::PostMaster().Subscribe(EMessageType::SetAmbience, this);

	//Pussel
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayDynamicAudioSource, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Play3DVoiceLine, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Play2DVoiceLine, this);

}

void CAudioManager::UnsubscribeToMessages()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::UIButtonPress, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayResearcherReactionExplosives, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayRobotAttackSound, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayRobotDeathSound, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayRobotIdleSound, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayRobotPatrolling, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayRobotSearching, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayVoiceEvent, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlaySFX, this);

	// Player & Gravity Glove 
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayStepSound, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayJumpSound, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerTakeDamage, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::GravityGlovePull, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::GravityGlovePush, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerHealthPickup, this);

	// Enemy
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyPatrolState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemySeekState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttackState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAlertedState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyIdleState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttack, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyTakeDamage, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyFoundPlayer, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyLostPlayer, this);

	//CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayVoiceLine, this);
	//CMainSingleton::PostMaster().Unsubscribe(EMessageType::StopDialogue, this);

	// Ambience
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Unsubscribe("Level_1-1", this);
	CMainSingleton::PostMaster().Unsubscribe("Level_1-2", this);
	CMainSingleton::PostMaster().Unsubscribe("Level_2-1", this);
	CMainSingleton::PostMaster().Unsubscribe("Level_2-2", this);

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::BootUpState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::GameStarted, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PauseMenu, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Resume, this);

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::AddStaticAudioSource, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::ClearStaticAudioSources, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetDynamicAudioSource, this);

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PhysicsPropCollision, this);

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyStateChange, this);

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetAmbience, this);
	
	//Pussel
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayDynamicAudioSource, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Play3DVoiceLine, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Play2DVoiceLine, this);
}

std::string CAudioManager::GetPath(EMusic type) const
{
	std::string path = myMusicPath;
	path.append(TranslateEnum(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(EAmbience type) const
{
	std::string path = myAmbiencePath;
	path.append(TranslateEnum(type));
	path.append(".wav");
	return path;
}

std::string CAudioManager::GetPath(EPropAmbience type) const
{
	std::string path = myPropAmbiencePath;
	path.append(TranslateEnum(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(ESFX type) const
{
	std::string path = mySFXPath;
	path.append(TranslateEnum(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(EUI type) const
{
	std::string path = myUIPath;
	path.append(TranslateEnum(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(EVOX type) const
{
	std::string path = myVoxPath;
	path.append(TranslateEnum(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::TranslateEnum(EChannel enumerator) const
{
	switch (enumerator)
	{
	case EChannel::Music:
		return "Music";
	case EChannel::Ambience:
		return "Ambience";
	case EChannel::SFX:
		return "SFX";
	case EChannel::UI:
		return "UI";
	case EChannel::VOX:
		return "VOX";
	case EChannel::DynamicChannel1:
		return "DynamicChannel1";
	case EChannel::DynamicChannel2:
		return "DynamicChannel2";
	case EChannel::DynamicChannel3:
		return "DynamicChannel3";
	default:
		return "";
	}
}

std::string CAudioManager::TranslateEnum(EMusic /*enumerator*/) const
{
	//switch (enumerator) {
	//default:
	//	return "";
	//}
	return "DE";
}

std::string CAudioManager::TranslateEnum(EAmbience enumerator) const {
	switch (enumerator)
	{
	case EAmbience::Cottage:
		return "Cottage";
	case EAmbience::Basement1:
		return "Basement1";
	case EAmbience::Basement2:
		return "Basement2";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(EPropAmbience enumerator) const
{
	switch (enumerator)
	{
	case EPropAmbience::GrandfatherClock:
		return "GrandfatherClock";
	case EPropAmbience::TVStatic:
		return "TVStatic";
	case EPropAmbience::ElectricalCabinet:
		return "ElectricalCabinet";
	case EPropAmbience::Boiler:
		return "Boiler";
	case EPropAmbience::Refrigerator:
		return "Refrigerator";
	case EPropAmbience::Toilet:
		return "Toilet";
	case EPropAmbience::PhoneCalling:
		return "PhoneCalling";
	case EPropAmbience::PhoneDead:
		return "PhoneDead";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(ESFX enumerator) const {
	switch (enumerator)
	{
	case ESFX::Unused:
		return "Grab";
	case ESFX::Unused1:
		return "Grab";
	case ESFX::Grab:
		return "Grab";
	case ESFX::Throw:
		return "Throw";
	case ESFX::LetGo:
		return "LetGo";
	case ESFX::EnemyHit:
		return "EnemyHit";
	case ESFX::SwitchPress:
		return "SwitchPress";
	case ESFX::PickupGravityGlove:
		return "PickupGravityGlove";
	case ESFX::PickupHeal:
		return "PickupHeal";
	case ESFX::EnemyAttack:
		return "EnemyBackToPatrol";
	case ESFX::CardboardBox:
		return "CardboardBox";
	case ESFX::MovePainting:
		return "MovePainting";
	case ESFX::DoorOpen:
		return "DoorOpen";
	case ESFX::PhoneDead:
		return "PhoneDead";
	case ESFX::WoodImpactMedium:
		return "WoodImpactMedium";
	case ESFX::WoodImpactSmall:
		return "WoodImpactSmall";
	case ESFX::CeramicImpactLarge:
		return "CeramicImpactLarge";
	case ESFX::CeramicImpactMedium:
		return "CeramicImpactMedium";
	case ESFX::CeramicImpactSmall:
		return "CeramicImpactSmall";
	case ESFX::PlasticImpact:
		return "PlasticImpact";
	case ESFX::PaperImpact:
		return "PaperImpact";
	case ESFX::SilverwareImpact:
		return "SilverwareImpact";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(ESFXCollection enumerator) const
{
	switch (enumerator)
	{
	case ESFXCollection::StepWood:
		return "StepWood";
	case ESFXCollection::StepWoodFast:
		return "StepWoodFast";
	case ESFXCollection::StepCarpet:
		return "StepCarpet";
	case ESFXCollection::StepCarpetFast:
		return "StepCarpetFast";
	case ESFXCollection::StepConcrete:
		return "StepConcrete";
	case ESFXCollection::StepConcreteFast:
		return "StepConcreteFast";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(EUI enumerator) const {
	switch (enumerator)
	{
	case EUI::ButtonClick:
		return "ButtonClick";
	case EUI::PlayClick:
		return "PlayClick";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(EVOX enumerator) const
{
	switch (enumerator)
	{
	case EVOX::Line0:
		return "0";
	case EVOX::Line1:
		return "1";
	case EVOX::Line2:
		return "2";
	case EVOX::Line3:
		return "3";
	case EVOX::Line4:
		return "4";
	case EVOX::Line5:
		return "5";
	case EVOX::Line6:
		return "6";
	case EVOX::Line7:
		return "7";
	case EVOX::Line8:
		return "8";
	case EVOX::Line9:
		return "9";
	case EVOX::Line10:
		return "10";
	case EVOX::Line11:
		return "11";
	case EVOX::Line12:
		return "12";
	case EVOX::Line13:
		return "13";
	case EVOX::Line14:
		return "14";
	case EVOX::Line15:
		return "15";
	case EVOX::Line16:
		return "16";
	case EVOX::Line17:
		return "17";
	case EVOX::Line18:
		return "18";
	case EVOX::Line19:
		return "19";
	case EVOX::Line20:
		return "20";
	case EVOX::Line21:
		return "21";
	case EVOX::Line22:
		return "22";
	case EVOX::Line23:
		return "23";
	case EVOX::Line24:
		return "24";
	case EVOX::Line25:
		return "25";
	case EVOX::Line26:
		return "26";
	case EVOX::Line27:
		return "27";
	case EVOX::Line28:
		return "28";
	case EVOX::Line29:
		return "29";
	case EVOX::Line30:
		return "30";
	case EVOX::Line31:
		return "31";
	case EVOX::Line32:
		return "32";
	case EVOX::Line33:
		return "33";
	case EVOX::Line34:
		return "34";
	case EVOX::Line35:
		return "35";
	case EVOX::Line36:
		return "36";
	case EVOX::Line37:
		return "37";
	case EVOX::Line38:
		return "38";
	case EVOX::Line39:
		return "39";
	case EVOX::Line40:
		return "40";
	case EVOX::Heal1:
		return "Heal1";
	case EVOX::Heal2:
		return "Heal2";
	case EVOX::Heal3:
		return "Heal3";
	case EVOX::LeaveCottage:
		return "LeaveCottage";
	case EVOX::PickUpPhone:
		return "PickUpPhone";
	case EVOX::WakeUpAfterDamage1:
		return "WakeUpAfterDamage1";
	case EVOX::WakeUpAfterDamage2:
		return "WakeUpAfterDamage2";
	case EVOX::WakeUpAfterDamage3:
		return "WakeUpAfterDamage3";
	default:
		return "";
	}
}

std::string CAudioManager::TranslateEnum(EEnemyVoiceLine enumerator) const
{
	switch (enumerator)
	{
	case EEnemyVoiceLine::EnemyDamagePlayer:
		return "EnemyDamagePlayer";
	case EEnemyVoiceLine::EnemyHeardNoise:
		return "EnemyHeardNoise";
	case EEnemyVoiceLine::EnemyLostPlayer:
		return "EnemyLostPlayer";
	case EEnemyVoiceLine::EnemyBackToPatrol:
		return "EnemyBackToPatrol";
	case EEnemyVoiceLine::EnemyPatrol:
		return "EnemyPatrol";
	case EEnemyVoiceLine::EnemyFoundPlayer:
		return "EnemyFoundPlayer";
	case EEnemyVoiceLine::EnemyChasing:
		return "EnemyChasing";
	default:
		return "";
	}
}

void CAudioManager::FillCollection(ESFXCollection enumerator)
{
	unsigned int counter = 0;

	switch (enumerator)
	{
	case ESFXCollection::StepWood:
	{
		CAudio* sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myWoodStepSounds.push_back(sound);
			sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));
		}
	}
	break;
	case ESFXCollection::StepWoodFast:
	{
		CAudio* sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myWoodFastStepSounds.push_back(sound);
			sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));
		}
	}
	break;
	case ESFXCollection::StepCarpet:
	{
		CAudio* sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myCarpetStepSounds.push_back(sound);
			sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));
		}
	}
	break;
	case ESFXCollection::StepCarpetFast:
	{
		CAudio* sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myCarpetFastStepSounds.push_back(sound);
			sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));
		}
	}
	break;
	case ESFXCollection::StepConcrete:
	{
		CAudio* sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myConcreteStepSounds.push_back(sound);
			sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));
		}
	}
	break;
	case ESFXCollection::StepConcreteFast:
	{
		CAudio* sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myConcreteFastStepSounds.push_back(sound);
			sound = myWrapper.TryGetSound(mySFXPath + GetCollectionPath(enumerator, ++counter));
		}
	}
	break;
	default:
		break;
	}
}

void CAudioManager::LoadEnemyLines()
{
	std::string path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(0)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path));

	path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(1)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path));

	path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(2)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path));

	path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(3)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path));

	path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(4)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path, true));

	path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(5)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path));

	path = myVoxPath;
	path.append(TranslateEnum(static_cast<EEnemyVoiceLine>(6)));
	path.append(".mp3");
	myEnemyVoiceSounds.push_back(myWrapper.RequestSound(path, true));
}

void CAudioManager::PlayRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel, const int& aMaxNrOfChannelsActive)
{
	if (aCollection.empty())
		return;

	//std::cout << __FUNCTION__ << " " << myChannels[CAST(aChannel)]->PlayCount() << std::endl;
	if (myChannels[CAST(aChannel)]->PlayCount() > aMaxNrOfChannelsActive)
		return;

	unsigned int randomIndex = Random(0, static_cast<int>(aCollection.size()) - 1);
	myWrapper.Play(aCollection[randomIndex], myChannels[CAST(aChannel)]);
}

void CAudioManager::PlayCyclicRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel, std::vector<int>& someCollectionIndices, const int& aMaxNrOfChannelsActive)
{
	if (aCollection.empty())
		return;

	if (myChannels[CAST(aChannel)]->PlayCount() > aMaxNrOfChannelsActive)
		return;

	unsigned int randomIndex = Random(0, static_cast<int>(aCollection.size()) - 1, someCollectionIndices);
	myWrapper.Play(aCollection[randomIndex], myChannels[CAST(aChannel)]);
}

void CAudioManager::FadeChannelOverSeconds(const EChannel& aChannel, const float& aNumberOfSeconds, const bool& aShouldFadeOut)
{
	if (aShouldFadeOut)
	{
		if (myChannels[CAST(aChannel)]->GetVolume() != 0.0f)
		{
			myFadingChannels.push_back({ aChannel, aNumberOfSeconds, aNumberOfSeconds, aShouldFadeOut });
		}
	}
	else 
	{
		if (myChannels[CAST(aChannel)]->GetVolume() != 1.0f)
		{
			myFadingChannels.push_back({ aChannel, aNumberOfSeconds, aNumberOfSeconds, aShouldFadeOut });
		}
	}
}

void CAudioManager::SetDynamicTrack(const EAmbience& aFirstTrack, const EAmbience& aSecondTrack, const EAmbience& aThirdTrack)
{
	myWrapper.Play(myAmbienceAudio[CAST(aFirstTrack)], myChannels[CAST(EChannel::DynamicChannel1)]);
	myWrapper.Play(myAmbienceAudio[CAST(aSecondTrack)], myChannels[CAST(EChannel::DynamicChannel2)]);
	myWrapper.Play(myAmbienceAudio[CAST(aThirdTrack)], myChannels[CAST(EChannel::DynamicChannel3)]);
}

void CAudioManager::Pause()
{
	myChannels[CAST(EChannel::Ambience)]->SetPaused(true);
	myChannels[CAST(EChannel::Music)]->SetPaused(true);
	myChannels[CAST(EChannel::SFX)]->SetPaused(true);
	myChannels[CAST(EChannel::VOX)]->SetPaused(true);
	myChannels[CAST(EChannel::DynamicChannel1)]->SetPaused(true);
	myChannels[CAST(EChannel::DynamicChannel2)]->SetPaused(true);
	myChannels[CAST(EChannel::DynamicChannel3)]->SetPaused(true);

	for (unsigned int i = 0; i < myStaticAudioSources.size(); ++i)
	{
		myStaticAudioSources[i].myChannel->SetPaused(true);
	}

	myDynamicSource->SetPaused(true);
}

void CAudioManager::Resume()
{
	myChannels[CAST(EChannel::Ambience)]->SetPaused(false);
	myChannels[CAST(EChannel::Music)]->SetPaused(false);
	myChannels[CAST(EChannel::SFX)]->SetPaused(false);
	myChannels[CAST(EChannel::VOX)]->SetPaused(false);
	myChannels[CAST(EChannel::DynamicChannel1)]->SetPaused(false);
	myChannels[CAST(EChannel::DynamicChannel2)]->SetPaused(false);
	myChannels[CAST(EChannel::DynamicChannel3)]->SetPaused(false);

	for (unsigned int i = 0; i < myStaticAudioSources.size(); ++i)
	{
		myStaticAudioSources[i].myChannel->SetPaused(false);
	}

	myDynamicSource->SetPaused(false);
}
