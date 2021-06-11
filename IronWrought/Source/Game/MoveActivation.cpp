#include "stdafx.h"
#include "MoveActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"

CMoveActivation::CMoveActivation(CGameObject& aParent, const SSettings& someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
{
	mySettings.myStartPosition = /*GameObject().myTransform->Position() +*/ mySettings.myStartPosition;
	mySettings.myEndPosition = /*GameObject().myTransform->Position() +*/ mySettings.myEndPosition;
}

CMoveActivation::~CMoveActivation()
{
}

void CMoveActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();
		float t = std::clamp((myTime / mySettings.myDuration), 0.0f, 1.0f);
		Vector3 position = Vector3::Lerp(mySettings.myStartPosition, mySettings.myEndPosition, t);
		GameObject().myTransform->Position(position);
	}

	if (Complete(myTime >= mySettings.myDuration))
	{	
		Vector3 temp = mySettings.myStartPosition;
		mySettings.myStartPosition = mySettings.myEndPosition;
		mySettings.myEndPosition = temp;
		myTime -= mySettings.myDuration;
	}
}