#pragma once
#include "ResponseBehavior.h"
class CTeleportResponse : public IResponseBehavior/*, public IObserver*/
{
public:
	CTeleportResponse(CGameObject& aParent, const float aDelay, const std::string& aTarget);
	~CTeleportResponse() override;

	void Update() override;

	void OnRespond() override;
	void Awake() override;
	void Start() override;

private:

	float myDelay;
	std::string myTarget;

	//void Receive(const SMessage& aMessage) override;
//
//private:
//	void HandleTeleport(CTransformComponent* aTargetToTeleport);
//
//private:
//	Vector3	myOnTeleportToMePosition;// 12
//	PostMaster::ELevelName myName;// 4
//	Vector3 myOnTeleportToMeRotation;// 12
//	PostMaster::ELevelName myTeleportTo;// 4
//	float myTeleportTimer;// 4
//	bool myHasTeleported;
//	bool myActivated;
};

