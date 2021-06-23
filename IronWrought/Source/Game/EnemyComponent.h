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
class CIdle;

namespace physx {
	class PxShape;
}

struct SEnemySetting {
	float mySpeed;	
	float myRadius;	
	float myAttackDistance; 

	std::vector<int> myPatrolGameObjectIds;
	std::vector<Vector3> mySpawnPoints;
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
		Detection,
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

	const float CurrentStateBlendValue() const;

	const bool MakesSound() const;

public:
	float WrapAngle(float anAngleRadians)
	{
		return fmodf(DirectX::XMConvertToDegrees(anAngleRadians), 360.0f);
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
	float SmoothStep(float a, float b, float t);
	float InverseLerp(float a, float b, float v);
	void UpdateVignette(const float aDotOverload = -1.0f);

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
	bool myHasScreamed;
	bool myMakesSound;

	float myCurrentStateBlend;
	float myCurrentVignetteBlend;
	float myTargetVignetteBlend;
	float mySqrdDistanceToPlayer;
	float myCloseToPlayerThreshold;
	float myAttackPlayerTimer;
	const float myAttackPlayerTimerMax;
	//float myLastSeenDistanceToPlayer;// Confusing name? The distance to the player 
	float myWakeUpTimer;
	float myIdlingTimer;
	SNavMesh* myNavMesh;
	CIdle* myIdleState;
	float myDetectionTimer;
	float myAggroTimer;
	float myAggroTime;
	float myDeAggroTimer;
	float myDeAggroTime;
	float myStepTimer;

	CTransformComponent* myDetachedPlayerHead;
	const float myGrabRange = 3.0f;
	const float myWalkSpeed;
	const float mySeekSpeed;
};
