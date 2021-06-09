#pragma once

class CTransformComponent;
class CAudioChannel;
namespace PostMaster
{
	enum class ELevelName
	{
		Cottage_1,
		Basement_1_1_A,
		Basement_1_1_B,
		Basement_2,
		Basement_1_2_A,
		Basement_1_2_B,
		Cottage_2,
		Basement_1_3,
		None
	};

	struct STeleportData
	{
		CTransformComponent* myTransformToTeleport = nullptr;
		ELevelName myTarget = ELevelName::None;
		bool myStartTeleport = false;

		void Reset()
		{
			myTransformToTeleport = nullptr;
			myTarget = ELevelName::None;
			myStartTeleport = false;
		}
	};

	struct SCrossHairData
	{
		enum class ETargetStatus
		{
			Targeted,
			Holding,
			None

		} myTargetStatus = ETargetStatus::None;
		// Animation data. TBD if used
		//unsigned int myIndex	= 0;
		//bool myShouldLoop		= false;
		//bool myShouldBeReversed = false;

	};

	struct SGravityGloveTargetData
	{
		float myInitialDistanceSquared = 0.0f;
		float myCurrentDistanceSquared = 0.0f;
	};

	struct SBoxColliderEvenTriggerData
	{
		bool myState = false;
		int mySceneSection = -1;
		CTransformComponent* myCollidersTransform = nullptr;
		CTransformComponent* myOthersTransform = nullptr;
	};

	struct SAudioSourceInitData
	{
		Vector3 myPosition;
		Vector3 myForward;
		float myStartAttenuationAngle;
		float myMaxAttenuationAngle;
		float myMinimumVolume;
		float myMinAttenuationDistance;
		float myMaxAttenuationDistance;
		int mySoundIndex;
		int myGameObjectID;
		float myDelay;
	};

	struct SStepSoundData
	{
		int myGroundMaterial;
		bool myIsSprint;
	};

	struct SPlayDynamicAudioData {
		int mySoundIndex;
		CAudioChannel* myChannel;
	};

	// SP7: Checks if the string is equivalent to any of our levels
	const bool LevelCheck(const char* aMessageToCompare);
}