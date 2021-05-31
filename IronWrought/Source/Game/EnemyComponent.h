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
	friend class CEnemyAudioComponent;
public:
	enum class EBehaviour {
		Patrol,
		Seek,
		Alerted,
		Attack,
		Count
	};

public:
	CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings);
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
	friend class CEnemyAnimationController;

	CCharacterController* myController;
	std::vector<CAIController*> myBehaviours;
	EBehaviour myCurrentState;
	CGameObject* myPlayer;
	SEnemySetting mySettings;
	float myCurrentOrientation; 
	CRigidBodyComponent* myRigidBodyComponent;

	bool myHasFoundPlayer;
	bool myHasReachedLastPlayerPosition;
	bool myMovementLocked;
	bool myHasReachedTarget;
	float myWakeUpTimer;
	SNavMesh* myNavMesh;
};
