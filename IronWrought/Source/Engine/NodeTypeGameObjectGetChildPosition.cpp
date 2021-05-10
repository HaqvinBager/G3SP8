#include "stdafx.h"
#include "NodeTypeGameObjectGetChildPosition.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "NodeDataManager.h"

CNodeTypeGameObjectGetChildPosition::CNodeTypeGameObjectGetChildPosition()
{
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3)); //0
}

int CNodeTypeGameObjectGetChildPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	int goid = CNodeDataManager::Get()->GetData<int>(myNodeName);
	auto obj = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(goid);
	Vector3 position = obj->myTransform->Position();

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &position, sizeof(Vector3));

	return -1;
}