#pragma once
#include "ActivationBehavior.h"
#include "Observer.h"
#include "PostMasterStructs.h"

class CTeleportActivation : public IActivationBehavior, public IObserver
{
public:
	CTeleportActivation(CGameObject& aParent
						, const PostMaster::ELevelName aNameOfTeleporter
						, const PostMaster::ELevelName aNameOfTeleportTo
						, const Vector3& aPosOnTeleportTo
						, const Vector3& aRotOnTeleportTo
						, const float aTimeUntilTeleport = 0.3f
	);
	~CTeleportActivation() override;

	void Update() override;

	void Receive(const SMessage& aMessage) override;

private:
	void HandleTeleport(CTransformComponent* aTargetToTeleport);

private:
	Vector3	myOnTeleportToMePosition;// 12
	PostMaster::ELevelName myName;// 4
	Vector3 myOnTeleportToMeRotation;// 12
	PostMaster::ELevelName myTeleportTo;// 4
	float myTeleportTimer;// 4
	bool myHasTeleported;
	bool myActivated;
};
 
