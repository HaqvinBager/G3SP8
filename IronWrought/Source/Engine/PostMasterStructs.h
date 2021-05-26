#pragma once

class CTransformComponent;
namespace PostMaster
{
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
		bool myState;
		CTransformComponent* myTransform;
	};

	struct SStaticAudioSourceInitData
	{
		Vector3 myPosition;
		Vector3 myForward;
		float myStartAttenuationAngle;
		float myMaxAttenuationAngle;
		float myMinimumVolume;
		int mySoundIndex;
		int myGameObjectID;
	};

	// SP7: Checks if the string is equivalent to any of our levels
	const bool LevelCheck(const char* aMessageToCompare);
}