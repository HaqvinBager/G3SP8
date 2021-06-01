#pragma once
#include "BoxColliderComponent.h"
#include "Behavior.h"
#include "Observer.h"

class CTeleporterComponent : public CBehavior, public IObserver
{
public:
	enum class ELevelName
	{
		Cottage_1,
		Cottage_2,
		Basement_1_1_A,
		Basement_1_1_B,
		Basement_1_2_A,
		Basement_1_2_B,
		Basement_1_3,
		Basement_2,
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

public:
	CTeleporterComponent(CGameObject& aParent
						 , const ELevelName aNameOfTeleporter
						 , const ELevelName aNameOfTeleportTo
						 , const Vector3& aPosOnTeleportTo
	);
	~CTeleporterComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void Receive(const SMessage& aMessage) override;

	void TriggerTeleport(const bool aEnterCollider, CTransformComponent* aTarget);

private:
	void HandleTeleport();

private:
	ELevelName myName;
	ELevelName myTeleportTo;
	Vector3 myOnTeleportToMe;

	float myTeleportTimer;
	const float myTimeUntilTeleport;
	bool myTeleportInProgress;
	CTransformComponent* myTeleportTarget;
};

