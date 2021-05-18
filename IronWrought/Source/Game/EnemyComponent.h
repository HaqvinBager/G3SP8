#pragma once
#include "Component.h"
#include "Observer.h"
#define PI 3.14159265f
class CAIController;
class CCharacterController;
class CRigidBodyComponent;
class CGameObject;
struct SNavMesh;

namespace physx {
	class PxShape;
}

struct SEnemySetting {
	float mySpeed;	//= 0.1f;
	float myRadius;	//= 10.0f;
	float myHealth; //= 10.0f
	float myAttackDistance; //2.0f

	std::vector<int> myPatrolGameObjectIds;
};

class CEnemyComponent : public CComponent, public IObserver, public IStringObserver
{
public:
	enum class EBehaviour {
		Patrol,
		Seek,
		Attack,
		TakeDamage,

		Count
	};

public:
	CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings);
	~CEnemyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void FixedUpdate() override;
	void TakeDamage(float aDamage);
	void SetState(EBehaviour aState);
	const EBehaviour GetState()const;
	void Receive(const SStringMessage& aMsg) override;
	void Receive(const SMessage& aMsg) override;

	void Dead();

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
	CGameObject* myEnemy;
	CGameObject* myPlayer;
	SEnemySetting mySettings;
	float myCurrentHealth;
	std::vector<Vector3> myPatrolPositions;
	Quaternion myPatrolRotation;
	Vector3 myCurrentDirection;
	float myCurrentOrientation; 
	CRigidBodyComponent* myRigidBodyComponent;

	bool myMovementLocked;
	float myWakeUpTimer;

	float myYaw;
	float myPitch;
	SNavMesh* myNavMesh;
};
