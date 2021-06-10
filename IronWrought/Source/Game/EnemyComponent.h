#pragma once
#include "Component.h"
#include "Observer.h"
#define PI 3.14159265f
class CAIController;
class CCharacterController;
class CRigidBodyComponent;
class CGameObject;
class CPatrolPointComponent;
struct SNavMesh;

namespace physx {
	class PxShape;
}

struct SEnemySetting {
	float mySpeed;	
	float myRadius;	
	float myAttackDistance; 

	std::vector<int> myPatrolGameObjectIds;
	std::vector<float> myPatrolIntrestValue;
};

struct SInterestPoints {

};

class CEnemyComponent : public CComponent, public IObserver, public IStringObserver
{
public:
	enum class EBehaviour {
		Patrol,
		Seek,
		Alerted,
		Idle,
		Attack,
		Count
	};

public:
	CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings, SNavMesh* aNavMesh);
	~CEnemyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void FixedUpdate() override;
	void SetState(EBehaviour aState);
	const EBehaviour GetState()const;
	void Receive(const SStringMessage& aMsg) override;
	void Receive(const SMessage& aMsg) override;
	//CPatrolPointComponent* FindBestPatrolPoint();

	// Based on threshold. 
	const float PercentileDistanceToPlayer() const;

	const float CurrentStateBlendValue() const { return myCurrentStateBlend; };

public:
	float WrapAngle(float anAngle)
	{
		return fmodf(anAngle, 360.0f);
	}

	float ToDegrees(float anAngleInRadians)
	{
		return anAngleInRadians * (180.0f / PI);
	}
	float Lerp(float a, float b, float t) const
	{
		return (1.0f - t) * a + b * t;
	}

private:
	void UpdateAttackEvent();

private:
	friend class CEnemyAnimationController;

	CCharacterController* myController;
	std::vector<CAIController*> myBehaviours;
	EBehaviour myCurrentState;
	CGameObject* myPlayer;
	SEnemySetting mySettings;
	float myCurrentOrientation; 
	CRigidBodyComponent* myRigidBodyComponent;
	Vector3 mySpawnPosition;

	bool myHasFoundPlayer;
	bool myHasReachedLastPlayerPosition;
	bool myMovementLocked;
	bool myHasReachedAlertedTarget;
	bool myHeardSound;
	bool myIsIdle;

	float myCurrentStateBlend;
	float mySqrdDistanceToPlayer;
	float myCloseToPlayerThreshold;
	float myAttackPlayerTimer;
	const float myAttackPlayerTimerMax;
	//float myLastSeenDistanceToPlayer;// Confusing name? The distance to the player 
	float myWakeUpTimer;
	float myIdlingTimer;
	SNavMesh* myNavMesh;
};
