#pragma once
#include "Behavior.h"

class CAIController;
struct SNavMesh;

class CChaseEventBehavior : public CBehavior
{
public:
	struct SSettings
	{
		Vector3 mySpawnPosition;
		float mySeekSpeed;
		float myDelay;
		const float myAttackPlayerTimerMax;
		const float myGrabRange;
	};

	CChaseEventBehavior(CGameObject& aParent, SSettings someSettings, SNavMesh* aNavMesh);
	~CChaseEventBehavior() override;

public:
	void Awake() override;
	void Start() override;
	void Update() override;

public:
	void OnEnable() override;
	void OnDisable() override;

private:
	void Vignette();
	void Shake();
	void ChangeState();
	void UpdateAttackEvent();

private:
	std::vector<CAIController*> myBehaviors;
	SSettings mySettings;
	SNavMesh* myNavMesh;
	CGameObject* myPlayer;
	CTransformComponent* myDetachedPlayerHead;
	float myAttackPlayerTimer;
	int myCurrentBehavior;
	bool myMovementLocked;
};

