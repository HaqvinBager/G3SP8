#include "stdafx.h"
#include "NodeTypeGameObjectGetPosition.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "GraphManager.h"

CNodeTypeGameObjectGetPosition::CNodeTypeGameObjectGetPosition()
{
    myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3)); //0
    //myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //1
	//myPins.push_back(SPin("Z", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //2
}

int CNodeTypeGameObjectGetPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();
	Vector3 position = gameObject->myTransform->Position();

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &position, sizeof(Vector3));
	//DeclareDataOnPinIfNecessary<float>(pins[1]);
	//memcpy(pins[1].myData, &position.y, sizeof(float));
	//DeclareDataOnPinIfNecessary<float>(pins[2]);
	//memcpy(pins[2].myData, &position.z, sizeof(float));

	return -1;
}