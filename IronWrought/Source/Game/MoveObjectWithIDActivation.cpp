#include "stdafx.h"
#include "MoveObjectWithIDActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Scene.h"

CMoveObjectWithIDActivation::CMoveObjectWithIDActivation(CGameObject& aParent, const SSettings& someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
	, myHasBeenDelayed(false)
{
	mySettings.myStartPosition = /*GameObject().myTransform->Position() +*/ mySettings.myStartPosition;
	mySettings.myEndPosition = /*GameObject().myTransform->Position() +*/ mySettings.myEndPosition;
}

CMoveObjectWithIDActivation::~CMoveObjectWithIDActivation()
{
}

void CMoveObjectWithIDActivation::Update()
{
	if (myIsInteracted)
	{
		if (myTime >= mySettings.myDelay)
		{
			myTime = 0.0f;
			myHasBeenDelayed = true;
		}

		myTime += CTimer::Dt();

		if (!myHasBeenDelayed)
			return;

		float t = std::clamp((myTime / mySettings.myDuration), 0.0f, 1.0f);
		Vector3 position = Vector3::Lerp(mySettings.myStartPosition, mySettings.myEndPosition, t);

		CGameObject* gameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myGOIDToMove);

		gameObject->myTransform->Position(position);
	}

	if (Complete(myTime >= mySettings.myDuration))
	{
		Vector3 temp = mySettings.myStartPosition;
		mySettings.myStartPosition = mySettings.myEndPosition;
		mySettings.myEndPosition = temp;
		myTime -= mySettings.myDuration;
	}
}