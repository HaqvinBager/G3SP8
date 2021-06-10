#pragma once
#include "ActivationBehavior.h"
#include "Observer.h"
#include "PostMasterStructs.h"

class CTeleportActivation : public IActivationBehavior/*, public IObserver*/
{
public:
	CTeleportActivation(CGameObject& aParent, const float aDelay, const std::string& aTarget);
	~CTeleportActivation() override;

	void Update() override;
	void OnActivation() override;

	//void Receive(const SMessage& aMessage) override;
private:
	//void HandleTeleport(CTransformComponent* aTargetToTeleport);

private:

	float myDelay;
	const std::string myTarget;

	//Vector3	myOnTeleportToMePosition;// 12
	//PostMaster::ELevelName myName;// 4
	//Vector3 myOnTeleportToMeRotation;// 12
	//PostMaster::ELevelName myTeleportTo;// 4
	//float myTeleportTimer;// 4
	//bool myHasTeleported;
	//bool myActivated;
};
 
