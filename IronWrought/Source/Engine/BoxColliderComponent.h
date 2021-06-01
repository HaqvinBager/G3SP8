#pragma once
#include "Behavior.h"

namespace physx
{
	class PxShape;
	class PxMaterial;
}

class CScene;
class CTeleporterComponent;

class CLineInstance;

#ifdef _DEBUG
#define DEBUG_COLLIDER_BOX // DOES NOT WORK PROPERLY, UPDATE TRANSFORM 
#endif

class CBoxColliderComponent : public CBehavior
{
public:
	enum class EEventFilter
	{
		PlayerOnly,
		EnemyOnly,
		ObjectOnly,
		Any,
		None
	};

public:
	CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize, const bool aIsTrigger, const unsigned int aLayerValue, physx::PxMaterial* aMaterial = nullptr);
	~CBoxColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void CreateBoxCollider();

	virtual void OnTriggerEnter(CTransformComponent* aOther);
	virtual void OnTriggerExit(CTransformComponent* aOther);
	void RegisterEventTriggerMessage(const std::string& aMessage);
	void RegisterEventTriggerFilter(const int& anEventFilter);
	void RegisterEventTriggerAudioIndex(const int& anIndex);
	void RegisterEventTriggerOnce(const bool& aTriggerOnce);
	void RegisterSceneSection(const int aLevelIndex);
	void CanBeDeactivated(const bool aCanBeDeactivated);
	//const SStringMessage& EventTriggerMessage() { return myTriggerMessage; }

	void OnEnable() override;
	void OnDisable() override;

private:
	void CreateColliderDraw(const float& aHalfSizeX, const float& aHalfSizeY, const float& aHalfSizeZ, const Vector3& aPosOffset);

private:
	physx::PxShape* myShape;
	physx::PxMaterial* myMaterial;
	Vector3 myPositionOffset;
	Vector3 myBoxSize;
	bool myIsTrigger;
	unsigned int myLayerValue;

	std::string myEventMessage;
	EEventFilter myEventFilter;
	int myAudioEventIndex;
	int mySceneSection;
	bool myHasTriggered;
	bool myTriggerOnce;
	bool myCanBeDeactivated;

#ifdef DEBUG_COLLIDER_BOX
	CLineInstance* myColliderDraw;
#endif
};
