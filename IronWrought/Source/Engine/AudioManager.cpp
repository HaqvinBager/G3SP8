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
		myAmbienceAudio.push_back(myWrapper.Request3DSound(GetPath(static_cast<EAmbience>(i)), true));
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

	for (unsigned int i = 0; i < static_cast<unsigned int>(EResearcherEventVoiceLine::Count); ++i)
	{
		myResearcherEventSounds.push_back(myWrapper.RequestSound(GetPath(static_cast<EResearcherEventVoiceLine>(i))));
	}

	for (unsigned int i = 0; i < static_cast<unsigned int>(EResearcherReactionVoiceLine::Count); ++i)
	{
		FillCollection(static_cast<EResearcherReactionVoiceLine>(i));
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
		myChannels[CAST(EChannel::ResearcherVOX)]->SetVolume(value);
	}
	if (volDoc.HasMember("RobotVoice"))
	{
		float value = volDoc["RobotVoice"].GetFloat();
		myChannels[CAST(EChannel::RobotVOX)]->SetVolume(value);
	}

	myDynamicSource = myWrapper.RequestAudioSource("Enemy");

	//SetDynamicTrack(EAmbience::DynamicTestDrums, EAmbience::DynamicTestGlitches, EAmbience::DynamicTestScreamer);

	//myChannels[CAST(EChannel::DynamicChannel1)]->SetVolume(myDynamicChannel1);
	//myChannels[CAST(EChannel::DynamicChannel2)]->SetVolume(myDynamicChannel2);
	//myChannels[CAST(EChannel::DynamicChannel3)]->SetVolume(myDynamicChannel3);
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
	for (auto& sound : myResearcherEventSounds)
	{
		delete sound;
		sound = nullptr;
	}
	for (auto& sound : myResearcherReactionsExplosives)
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

	case EMessageType::PlayJumpSound:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::Jump)], myChannels[CAST(EChannel::SFX)]);
	}
	break;

	case EMessageType::PlayerTakeDamage:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::EnemyHit)], myChannels[CAST(EChannel::SFX)]);
	}
	break;

	case EMessageType::PlayResearcherReactionExplosives:
	{
		PlayRandomSoundFromCollection(myResearcherReactionsExplosives, EChannel::ResearcherVOX);
	}
	break;

	case EMessageType::PlayRobotAttackSound:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotAttackSounds, EChannel::RobotVOX, myAttackSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}
	break;

	case EMessageType::PlayRobotDeathSound:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotDeathSounds, EChannel::RobotVOX, myDeathSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}
	break;

	case EMessageType::PlayRobotIdleSound:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotIdleSounds, EChannel::RobotVOX, myIdleSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}
	break;

	case EMessageType::PlayRobotPatrolling:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotPatrollingSounds, EChannel::RobotVOX, myPatrollingSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}
	break;

	case EMessageType::PlayRobotSearching:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotSearchingSounds, EChannel::RobotVOX, mySearchingSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}
	break;

	case EMessageType::EnemyTakeDamage:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotDamageSounds, EChannel::RobotVOX, myDamageSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
		myWrapper.Play(mySFXAudio[CAST(ESFX::EnemyHit)], myChannels[CAST(EChannel::SFX)]);
	}
	break;

	case EMessageType::PlayResearcherEvent:
	{
		int index = *static_cast<int*>(aMessage.data);
		myChannels[CAST(EChannel::RobotVOX)]->Stop();
		myChannels[CAST(EChannel::ResearcherVOX)]->Stop();
		myWrapper.Play(myResearcherEventSounds[index], myChannels[CAST(EChannel::ResearcherVOX)]);
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
			myWrapper.Play(mySFXAudio[CAST(ESFX::GravityGlovePullRelease)], myChannels[CAST(EChannel::SFX)]);
		else
			myWrapper.Play(mySFXAudio[CAST(ESFX::GravityGlovePullHit)], myChannels[CAST(EChannel::SFX)]);
	}break;

	case EMessageType::GravityGlovePush:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::GravityGlovePush)], myChannels[CAST(EChannel::SFX)]);
	}break;

	case EMessageType::PlayerHealthPickup:
	{
		myWrapper.Play(mySFXAudio[CAST(ESFX::PickupHeal)], myChannels[CAST(EChannel::SFX)]);
	}break;


	case EMessageType::EnemyAttackState:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;

		PlayCyclicRandomSoundFromCollection(myRobotAttackSounds, EChannel::RobotVOX, myAttackSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}break;

	case EMessageType::EnemyPatrolState:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;

		PlayCyclicRandomSoundFromCollection(myRobotPatrollingSounds, EChannel::RobotVOX, myPatrollingSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);

	}break;

	case EMessageType::EnemySeekState:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;
		PlayCyclicRandomSoundFromCollection(myRobotAttackSounds, EChannel::RobotVOX, myAttackSoundIndices, AUDIO_MAX_NR_OF_SFX_FROM_COLLECTION);
	}break;

	case EMessageType::EnemyAttack:
	{
		if (myChannels[CAST(EChannel::ResearcherVOX)]->IsPlaying())
			return;

		if (mySFXAudio[CAST(ESFX::EnemyAttack)])
			myWrapper.Play(mySFXAudio[CAST(ESFX::EnemyAttack)], myChannels[CAST(EChannel::SFX)]);
	}break;

	case EMessageType::GameStarted:
	{
		myWrapper.Play(myResearcherEventSounds[CAST(EResearcherEventVoiceLine::V1)], myChannels[CAST(EChannel::ResearcherVOX)]);

	}break;

	case EMessageType::StartGame:
	{
		std::string scene = *reinterpret_cast<std::string*>(aMessage.data);
		if (strcmp(scene.c_str(), "VerticalSlice") == 0)
		{
			myChannels[CAST(EChannel::Ambience)]->Stop();
			myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
			return;
		}

		if (strcmp(scene.c_str(), "Level_1-1") == 0)
		{
			myChannels[CAST(EChannel::Ambience)]->Stop();
			myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
			return;
		}

		if (strcmp(scene.c_str(), "Level_1-2") == 0)
		{
			myChannels[CAST(EChannel::Ambience)]->Stop();
			myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
			return;
		}

		if (strcmp(scene.c_str(), "Level_2-1") == 0)
		{
			myChannels[CAST(EChannel::Ambience)]->Stop();
			myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
			return;
		}

		if (strcmp(scene.c_str(), "Level_2-2") == 0)
		{
			myChannels[CAST(EChannel::Ambience)]->Stop();
			myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
			return;
		}
	}break;

	case EMessageType::BootUpState:
	{
		myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		myWrapper.Play(myResearcherEventSounds[CAST(EResearcherEventVoiceLine::BootUp)], myChannels[CAST(EChannel::ResearcherVOX)]);
	}break;

	case EMessageType::MainMenu:
	{
		myChannels[CAST(EChannel::Ambience)]->Stop();
		myChannels[CAST(EChannel::Music)]->Stop();
		myChannels[CAST(EChannel::ResearcherVOX)]->Stop();
		myChannels[CAST(EChannel::RobotVOX)]->Stop();
		myChannels[CAST(EChannel::SFX)]->Stop();
		myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
	}break;

	case EMessageType::AddStaticAudioSource:
	{
		PostMaster::SStaticAudioSourceInitData data = *reinterpret_cast<PostMaster::SStaticAudioSourceInitData*>(aMessage.data);

		if (data.mySoundIndex < 0 || data.mySoundIndex >= static_cast<int>(EPropAmbience::Count))
			return;

		this->AddSource(data.myGameObjectID, data.mySoundIndex, data.myPosition, data.myForward, data.myStartAttenuationAngle, data.myMaxAttenuationAngle, data.myMinimumVolume);
	}break;

	case EMessageType::SetDynamicAudioSource:
	{
		CGameObject* data = reinterpret_cast<CGameObject*>(aMessage.data);
		myDynamicObject = data;
	}break;

	case EMessageType::PhysicsPropCollision:
	{
		unsigned int soundIndex = *reinterpret_cast<unsigned int*>(aMessage.data);
		myChannels[CAST(EChannel::SFX)]->SetPitch(Random(0.95f, 1.05f));
		myWrapper.Play(mySFXAudio[soundIndex], myChannels[CAST(EChannel::SFX)]);
	}break;
	
	case EMessageType::PauseMenu:
	{
		Pause();
	}break;

	case EMessageType::Resume:
	{
		Resume();
	}break;

	case EMessageType::EnemyStateChange:
	{
		unsigned int state = *reinterpret_cast<unsigned int*>(aMessage.data);
		std::cout << "Enemy State: " << state << std::endl;
	}break;

	default: break;
	}
}

void CAudioManager::Receive(const SStringMessage& aMessage)
{
	//Puzzle Clips
	//

	//myWrapper.Play(myAmbienceAudio[0], myChannels[0]);


	if (strcmp(aMessage.myMessageType, "Level_1-1") == 0)
	{
		myChannels[CAST(EChannel::Ambience)]->Stop();
		myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		return;
	}

	if (strcmp(aMessage.myMessageType, "Level_1-2") == 0)
	{
		myChannels[CAST(EChannel::Ambience)]->Stop();
		myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Inside)], myChannels[CAST(EChannel::Ambience)]);
		return;
	}

	if (strcmp(aMessage.myMessageType, "Level_2-1") == 0)
	{
		myChannels[CAST(EChannel::Ambience)]->Stop();
		myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
		return;
	}

	if (strcmp(aMessage.myMessageType, "Level_2-2") == 0)
	{
		myChannels[CAST(EChannel::Ambience)]->Stop();
		myWrapper.Play(myAmbienceAudio[CAST(EAmbience::Outside)], myChannels[CAST(EChannel::Ambience)]);
		return;
	}

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
	}

	myWrapper.Update();

	if (myDelayedSFX.size() > 0)
	{
		const float dt = CTimer::Dt();

		for (auto it = myDelayedSFX.begin(); it != myDelayedSFX.end();)
		{
			it->myTimer -= dt;
			if (it->myTimer <= 0.0f)
			{
				myWrapper.Play(mySFXAudio[CAST(it->mySFX)], myChannels[CAST(EChannel::SFX)]);
				it = myDelayedSFX.erase(it);
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

void CAudioManager::AddSource(const int anIdentifier, const unsigned int aSoundIndex, const Vector3& aPosition, const Vector3& aDirection, float aStartAttenuationAngle, float aMaxAttenuationAngle, float aMinimumVolume)
{
	myStaticAudioSources.push_back({ anIdentifier, aSoundIndex, myWrapper.RequestAudioSource("3D") });
	myStaticAudioSources.back().myChannel->Set3DAttributes(aPosition, Vector3::Zero);
	myStaticAudioSources.back().myChannel->Set3DConeAttributes(aDirection, aStartAttenuationAngle, aMaxAttenuationAngle, aMinimumVolume);
	myWrapper.Play(myPropAmbienceAudio[aSoundIndex], myStaticAudioSources.back().myChannel);
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
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayResearcherEvent, this);
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
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyTakeDamage, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttack, this);

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

	//Pussel
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
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayResearcherEvent, this);
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
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyTakeDamage, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttack, this);

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
	path.append(".mp3");
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

std::string CAudioManager::GetPath(EResearcherEventVoiceLine type) const
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
	case EChannel::ResearcherVOX:
		return "ResearcherVOX";
	case EChannel::RobotVOX:
		return "RobotVOX";
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
	case EAmbience::AirVent:
		return "AirVent";
	case EAmbience::Inside:
		return "Inside";
	case EAmbience::Outside:
		return "Outside";
	case EAmbience::EnemyArea:
		return "EnemyArea";
	case EAmbience::DynamicTestDrums:
		return "DynamicTestDrums";
	case EAmbience::DynamicTestGlitches:
		return "DynamicTestGlitches";
	case EAmbience::DynamicTestScreamer:
		return "DynamicTestScreamer";
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
	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(ESFX enumerator) const {
	switch (enumerator)
	{
	case ESFX::GravityGlovePullBuildup:
		return "GravityGlovePullBuildup";
	case ESFX::GravityGlovePullHit:
		return "GravityGlovePullHit";
	case ESFX::GravityGlovePush:
		return "GravityGlovePush";
	case ESFX::GravityGlovePullRelease:
		return "GravityGlovePullRelease";
	case ESFX::Jump:
		return "Jump";
	case ESFX::EnemyHit:
		return "EnemyHit";
	case ESFX::SwitchPress:
		return "SwitchPress";
	case ESFX::PickupGravityGlove:
		return "PickupGravityGlove";
	case ESFX::PickupHeal:
		return "PickupHeal";
	case ESFX::EnemyAttack:
		return "EnemyAttack";
	case ESFX::CardboardBox:
		return "CardboardBox";
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
std::string CAudioManager::TranslateEnum(EResearcherEventVoiceLine enumerator) const
{
	switch (enumerator)
	{
	case EResearcherEventVoiceLine::V1:
		return "V1";
	case EResearcherEventVoiceLine::V2:
		return "V2";
	case EResearcherEventVoiceLine::BootUp:
		return "BootUp";
	case EResearcherEventVoiceLine::Intro:
		return "Intro";
	case EResearcherEventVoiceLine::Line1:
		return "1";
	case EResearcherEventVoiceLine::Line2:
		return "2";
	case EResearcherEventVoiceLine::Line3:
		return "3";
	case EResearcherEventVoiceLine::Line4:
		return "4";
	case EResearcherEventVoiceLine::Line5:
		return "5";
	case EResearcherEventVoiceLine::Line6:
		return "6";
	case EResearcherEventVoiceLine::Line7:
		return "7";
	case EResearcherEventVoiceLine::Line8:
		return "8";
	case EResearcherEventVoiceLine::Line9:
		return "9";
	case EResearcherEventVoiceLine::Line10:
		return "10";
	case EResearcherEventVoiceLine::Line11:
		return "11";
	case EResearcherEventVoiceLine::Line12:
		return "12";
	case EResearcherEventVoiceLine::Line13:
		return "13";
	case EResearcherEventVoiceLine::Line14:
		return "14";
	case EResearcherEventVoiceLine::Line15:
		return "15";
	case EResearcherEventVoiceLine::Line16:
		return "16";
	case EResearcherEventVoiceLine::Line17:
		return "17";
	case EResearcherEventVoiceLine::Line18:
		return "18";
	case EResearcherEventVoiceLine::Line19:
		return "19";
	case EResearcherEventVoiceLine::Line20:
		return "20";
	case EResearcherEventVoiceLine::Line21:
		return "21";
	case EResearcherEventVoiceLine::Line22:
		return "22";
	case EResearcherEventVoiceLine::Line23:
		return "23";
	case EResearcherEventVoiceLine::Line24:
		return "24";
	case EResearcherEventVoiceLine::Line25:
		return "25";
	case EResearcherEventVoiceLine::Line26:
		return "26";
	case EResearcherEventVoiceLine::Line27:
		return "27";
	case EResearcherEventVoiceLine::Outro1:
		return "Outro1";
	case EResearcherEventVoiceLine::Outro2:
		return "Outro2";
	case EResearcherEventVoiceLine::Outro3:
		return "Outro3";
	case EResearcherEventVoiceLine::Outro4:
		return "Outro4";
	case EResearcherEventVoiceLine::Outro5:
		return "Outro5";
	case EResearcherEventVoiceLine::Outro6:
		return "Outro6";
	case EResearcherEventVoiceLine::Outro7:
		return "Outro7";

	default:
		return "";
	}
}
std::string CAudioManager::TranslateEnum(EResearcherReactionVoiceLine enumerator) const
{
	switch (enumerator)
	{
	case EResearcherReactionVoiceLine::ResearcherReactionExplosives:
		return "ResearcherReactionExplosives";
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

void CAudioManager::FillCollection(EResearcherReactionVoiceLine enumerator)
{
	unsigned int counter = 0;

	switch (enumerator)
	{
	case EResearcherReactionVoiceLine::ResearcherReactionExplosives:
	{
		CAudio* sound = myWrapper.TryGetSound(myVoxPath + GetCollectionPath(enumerator, ++counter));

		while (sound != nullptr)
		{
			myResearcherReactionsExplosives.push_back(sound);
			sound = myWrapper.TryGetSound(myVoxPath + GetCollectionPath(enumerator, ++counter));
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
	myFadingChannels.push_back({ aChannel, aNumberOfSeconds, aNumberOfSeconds, aShouldFadeOut });
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
	myChannels[CAST(EChannel::ResearcherVOX)]->SetPaused(true);
	myChannels[CAST(EChannel::RobotVOX)]->SetPaused(true);
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
	myChannels[CAST(EChannel::ResearcherVOX)]->SetPaused(false);
	myChannels[CAST(EChannel::RobotVOX)]->SetPaused(false);
	myChannels[CAST(EChannel::DynamicChannel1)]->SetPaused(false);
	myChannels[CAST(EChannel::DynamicChannel2)]->SetPaused(false);
	myChannels[CAST(EChannel::DynamicChannel3)]->SetPaused(false);

	for (unsigned int i = 0; i < myStaticAudioSources.size(); ++i)
	{
		myStaticAudioSources[i].myChannel->SetPaused(false);
	}

	myDynamicSource->SetPaused(false);
}
