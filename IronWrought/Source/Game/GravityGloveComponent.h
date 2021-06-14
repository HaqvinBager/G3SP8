#pragma once
#include "Behavior.h"
//#include <GravityGloveEditor.h>
#include "Observer.h"

class CTransformComponent;
class CRigidBodyComponent;

namespace ImGui {
	class CGravityGloveEditor;
}

namespace physx {
	class PxRigidStatic;
	class PxD6Joint;
	class PxRigidActor;
}

struct SGravityGloveSettings {
	float myPushForce; //Current Default Value: 27
	float myDistanceToMaxLinearVelocity;

	float myMaxPushForce;
	float myMinPushForce;

	float myMaxPullForce;
	float myMinPullForce;

	float myMaxDistance;
	float myMinDistance;
	float myGrabRange;
	float myStuckRange;
	float myCurrentDistanceInverseLerp;
};

struct SRigidBodyTarget
{
	CRigidBodyComponent* myRigidBodyPtr = nullptr;
	float initialDistanceSquared = 0.0f;
	float currentDistanceSquared = 0.0f;

	void Clear()
	{
		myRigidBodyPtr = nullptr;
		initialDistanceSquared = 0.0f;
		currentDistanceSquared = 0.0f;
	}
};

class CGravityGloveComponent : public CBehavior, public IStringObserver
{
	friend class ImGui::CGravityGloveEditor;
public:
	CGravityGloveComponent(CGameObject& aParent, CTransformComponent* aGravitySlot);
	~CGravityGloveComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void Receive(const SStringMessage& aMessage) override;

	CRigidBodyComponent* GetHoldingObject() { return myCurrentTarget.myRigidBodyPtr; }

private:
	// USE EITHER InteractionLogicContinuous OR InteractionLogicOnInput. Not both!
	void InteractionLogicContinuous();
	// USE EITHER InteractionLogicContinuous OR InteractionLogicOnInput. Not both!
	void InteractionLogicOnInput();

	//Actor0 and Actor1 is what the joint should be connected to
	//Actor0 will always be player and Actor1 will always be the object
	//Offest in forward (Z)
	physx::PxD6Joint* CreateD6Joint(physx::PxRigidActor* actor0, physx::PxRigidActor* actor1, float aOffest = 0.f);

	void RotateObject();

	void Pull();
	void Pull(CTransformComponent* aTransform, CRigidBodyComponent* aRigidBodyTarget);
	void Release();
	void Push();
	void Push(CTransformComponent* aTransform, CRigidBodyComponent* aRigidBodyTarget);
	float WrapAngle(float anAngle)
	{
		return fmodf(anAngle, 360.0f);
	}

	float ToDegrees(float anAngleInRadians)
	{
		return anAngleInRadians * (180.0f / 3.14159265f);
	}
private:
	SGravityGloveSettings mySettings;
	float InverseLerp(float a, float b, float t) const
	{
		if (a == b)
		{
			if (t < a)
			{
				return 0.f;
			}
			else
			{
				return 1.f;
			}
		}

		if (a > b)
		{
			float temp = b;
			b = a;
			a = temp;
		}

		return (t - a) / (b - a);
	}

	float Lerp(float a, float b, float t) const
	{
		//if (t < a)
		//{
		//	return 0.f;
		//}
		//else
		//{
		//	return 1.f;
		//}

		//if (a > b)
		//{
		//	float temp = b;
		//	b = a;
		//	a = temp;
		//}

		return (1.0f - t) * a + b * t;
		//return a + t * (b - a);
	}

	float Remap(const float iMin, const float iMax, const float oMin, const float oMax, const float v) const
	{
		float t = InverseLerp(iMin, iMax, v);
		return Lerp(oMin, oMax, t);
	}

	SRigidBodyTarget myCurrentTarget;
	CTransformComponent* myGravitySlot;
	//CRigidBodyComponent* myCurrentTarget;
	physx::PxRigidStatic* myRigidStatic;
	physx::PxD6Joint* myJoint;
	PostMaster::SCrossHairData myCrosshairData;
	bool myIsRotatingmode;
	bool myHoldingAItem;
	float myExtendedOffsetArm;
	Vector2 myObjectRotation;
};
