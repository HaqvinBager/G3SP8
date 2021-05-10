#include "stdafx.h"
#include "NodeTypeStartSphereOnTriggerStay.h"
#include "NodeType.h"
#include "NodeInstance.h"
#include "Engine.h"
#include "Scene.h"

CNodeTypeStartSphereOnTriggerStay::CNodeTypeStartSphereOnTriggerStay()
{
	myPins.push_back(SPin("Radius", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
}

int CNodeTypeStartSphereOnTriggerStay::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();
	Vector3 position = gameObject->myTransform->Position();

	if (CEngine::GetInstance()->GetActiveScene().Player())
	{
		Vector3 playerPosition = CEngine::GetInstance()->GetActiveScene().Player()->myTransform->Position();

		SPin::EPinType outType;
		NodeDataPtr someData = nullptr;
		size_t outSize = 0;

		GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
		float radius = NodeData::Get<float>(someData);
		radius *= radius;
		float distance = Vector3::DistanceSquared(playerPosition, position);
		aTriggeringNodeInstance->myShouldTriggerAgain = true;

		if (distance < radius)
			return 1;
		else
			return -1;
	}
	else
	{
		return -1;
	}

}
