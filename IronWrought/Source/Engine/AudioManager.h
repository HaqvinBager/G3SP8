#pragma once
#include <vector>
#include <memory>
#include "FModWrapper.h"
#include "Observer.h"

class CAudio;
class CAudioChannel;

//START

enum class EMusic { Count };
enum class EAmbience { Cottage1, Cottage2, Basement1, Basement2, Count };
enum class EPropAmbience { GrandfatherClock, TVStatic, ElectricalCabinet, Boiler, Refrigerator, Toilet, WaterFlow, WaterDrip, Count };
enum class ESFX { Unused, Grab, Throw, LetGo, Unused1, EnemyHit, SwitchPull, PickupGravityGlove, PickupHeal, EnemyAttack, CardboardBox, MovePainting, DoorOpen, PhoneDead, WoodImpactMedium, WoodImpactSmall, CeramicImpactLarge, CeramicImpactMedium, CeramicImpactSmall, PlasticImpact, PaperImpact, SilverwareImpact, AshtrayImpact, CanImpact, BookImpact, ToiletPaperRollImpact, CameraImpact, ViolinImpact, DoorRattle, PuzzleProgress, PuzzleSolved, PuzzleKey, InsertFuse, MechanicalDoorOpen, DoorCreak, DoorClose, ChestCreak, MovePainting2, MovePainting3, InsertHandle, FastDoorOpen, PizzaImpact, Count };
enum class ESFXCollection { StepWood, StepWoodFast, StepCarpet, StepCarpetFast, StepConcrete, StepConcreteFast, StepEnemy, StepEnemyFast, Count };
enum class EUI { ButtonClick, PlayClick, HoverButton, CameraWoosh, Count };
enum class EVOX { Line0, Line1, Line2, Line3, Line4, Line5, Line6, Line7, Line8, Line9, Line10, Line11, Line12, Line13, Line14, Line15, Line16, Line17, Line18, Line19, Line20, Line21, Line22, Line23, Line24, Line25, Line26, Line27, Line28, Line29, Line30, Line31, Line32, Line33, Line34, Line35, Line36, Line37, Line38, Line39, Line40, Heal1, Heal2, Heal3, LeaveCottage, PickUpPhone, WakeUpAfterDamage1, WakeUpAfterDamage2, WakeUpAfterDamage3, Lee1Tip, Lee2Tip, Lee3Tip, Count };
enum class EEnemyVoiceLine { EnemyDamagePlayer, EnemyHeardNoise, EnemyLostPlayer, EnemyBackToPatrol, EnemyPatrol, EnemyFoundPlayer, EnemyChasing, Count };
enum class EChannel { Music, Ambience, SFX, UI, VOX, DynamicChannel1, DynamicChannel2, DynamicChannel3, DynamicChannel4, Count };
enum class EGroundType { Concrete, AirVent, Count };
enum class EDialogueScene { EnterCottage1, BasementNoHandle, HouseSitting, Lee1, ChestSymbols, Lee2, DoorHandle, Lee3, EnterBasement1, EnemyIntroduction, NoticeElectricalMechanism, NoticeExit, NoticeExit2, EnterBasement2, FindKey, EnterCottage2, ShoutingForLee, EnterBedroom, TryingMainExit, UsingFuse, EnterBasement1Final, Outro, Heal1, Heal2, Heal3, TryLeaveCottage1, PickupPhone, WakeUpAfterDamage1, WakeUpAfterDamage2, WakeUpAfterDamage3, Lee1Tip, Lee2Tip, Lee3Tip, Count};

//STOP

class CAudioManager : public IObserver, public IStringObserver {
public:
	CAudioManager();
	~CAudioManager();

	void Receive(const SMessage& aMessage);
	void Receive(const SStringMessage& aMessage);

	void Update();

	void SetListener(CGameObject* aGameObject);

	CAudioChannel* AddSource(const PostMaster::SAudioSourceInitData& someData);
private:
	void AddStaticSource(const PostMaster::SAudioSourceInitData& someData);
	void RemoveSource(const int anIdentifier);
	void ClearSources();

private:
	void SubscribeToMessages();
	void UnsubscribeToMessages();

	std::string GetPath(EMusic type) const;
	std::string GetPath(EAmbience type) const;
	std::string GetPath(EPropAmbience type) const;
	std::string GetPath(ESFX type) const;
	std::string GetPath(EUI type) const;
	std::string GetPath(EVOX type) const;

	template <class T>
	std::string GetCollectionPath(T enumerator, unsigned int anIndex) const;

	std::string TranslateEnum(EChannel enumerator) const;
	std::string TranslateEnum(EMusic enumerator) const;
	std::string TranslateEnum(EAmbience enumerator) const;
	std::string TranslateEnum(EPropAmbience enumerator) const;
	std::string TranslateEnum(ESFX enumerator) const;
	std::string TranslateEnum(ESFXCollection enumerator) const;
	std::string TranslateEnum(EUI enumerator) const;
	std::string TranslateEnum(EVOX enumerator) const;
	std::string TranslateEnum(EEnemyVoiceLine enumerator) const;

	void FillCollection(ESFXCollection enumerator);
	void LoadEnemyLines();

private:
	void PlayRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel, const int& aMaxNrOfChannelsActive = 5);
	void PlayCyclicRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel, std::vector<int>& someCollectionIndices, const int& aMaxNrOfChannelsActive = 5);
	void PlayCyclicRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, CAudioChannel* aChannel, std::vector<int>& someCollectionIndices, const int& aMaxNrOfChannelsActive = 2);

	void FadeChannelOverSeconds(const EChannel& aChannel, const float& aNumberOfSeconds, const bool& aShouldFadeOut = true);
	void SetDynamicTrack(const EAmbience& aFirstTrack, const EAmbience& aSecondTrack, const EAmbience& aThirdTrack, const EAmbience& aFourthTrack);

	void Pause();
	void Resume();

private:
	const std::string& myAmbiencePath = "Audio/Ambience/";
	const std::string& myPropAmbiencePath = "Audio/PropAmbience/";
	const std::string& myMusicPath = "Audio/Music/";
	const std::string& mySFXPath = "Audio/SFX/";
	const std::string& myUIPath = "Audio/UI/";
	const std::string& myVoxPath = "Audio/VOX/";
	
	EGroundType myCurrentGroundType;

	CFModWrapper myWrapper;

	std::vector<CAudio*> myAmbienceAudio;
	std::vector<CAudio*> myPropAmbienceAudio;
	std::vector<CAudio*> myMusicAudio;
	std::vector<CAudio*> mySFXAudio;
	std::vector<CAudio*> myUIAudio;

	std::vector<CAudio*> myWoodStepSounds;
	std::vector<CAudio*> myWoodFastStepSounds;
	std::vector<CAudio*> myCarpetStepSounds;
	std::vector<CAudio*> myCarpetFastStepSounds;
	std::vector<CAudio*> myConcreteStepSounds;
	std::vector<CAudio*> myConcreteFastStepSounds;
	std::vector<CAudio*> myEnemyStepSounds;
	std::vector<CAudio*> myEnemyFastStepSounds;

	std::vector<CAudio*> myVoiceEventSounds;

	std::vector<CAudio*> myEnemyVoiceSounds;

	std::vector<CAudio*> myRobotAttackSounds;
	std::vector<CAudio*> myRobotDamageSounds;
	std::vector<CAudio*> myRobotDeathSounds;
	std::vector<CAudio*> myRobotIdleSounds;
	std::vector<CAudio*> myRobotPatrollingSounds;
	std::vector<CAudio*> myRobotSearchingSounds;

	std::vector<int> myStepSoundIndices;
	std::vector<int> myAttackSoundIndices;
	std::vector<int> myDamageSoundIndices;
	std::vector<int> myDeathSoundIndices;
	std::vector<int> myIdleSoundIndices;
	std::vector<int> myPatrollingSoundIndices;
	std::vector<int> mySearchingSoundIndices;
	std::vector<int> myEnemyStepSoundIndices;

	std::vector<CAudioChannel*> myChannels;

	CGameObject* myListener;

	CGameObject* myDynamicObject;
	std::vector<CGameObject*> myDynamicObjects;

	CAudioChannel* myDynamicSource;
	std::vector<CAudioChannel*> myDynamicSources;

	CAudioChannel* my3DVoiceSource;

	float myDynamicChannel1;
	float myDynamicChannel2;
	float myDynamicChannel3;
	float myDynamicChannel4;

	struct SDelayedAudio
	{
		CAudio* myAudio;
		CAudioChannel* myChannel;
		float myTimer = 0.0f;	
	};
	std::vector<SDelayedAudio> myDelayedAudio;

	struct SFadingChannel
	{
		EChannel myChannel;
		float myDuration = 0.0f;
		float myTimer = 0.0f;
		bool myFadeOut = true;
	};
	std::vector<SFadingChannel> myFadingChannels;

	struct SStaticAudioSource
	{
		//SStaticAudioSource();
		//SStaticAudioSource(int aGameObjectID, unsigned int aSoundIndex, CAudioChannel* anAudioChannel);
		//SStaticAudioSource(SStaticAudioSource&& other) noexcept;
		//SStaticAudioSource& operator=(SStaticAudioSource&& other) noexcept;
		//SStaticAudioSource(SStaticAudioSource& other) noexcept;
		//SStaticAudioSource& operator=(SStaticAudioSource& other) noexcept;
		//~SStaticAudioSource();
		int myGameObjectID;
		unsigned int mySoundIndex;
		CAudioChannel* myChannel;
	};
	std::vector<SStaticAudioSource> myStaticAudioSources;
};

template<class T>
inline std::string CAudioManager::GetCollectionPath(T enumerator, unsigned int anIndex) const
{
	std::string path = "";
	path.append(TranslateEnum(enumerator));
	path.append(std::to_string(anIndex));
	path.append(".mp3");
	return path;
}
