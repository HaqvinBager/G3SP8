#include "stdafx.h"
#include "MoveActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"

CMoveActivation::CMoveActivation(CGameObject& aParent, const SSettings& someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myIsInteracted(false)
	, myTime(0.0f)
{
	mySettings.myStartPosition = GameObject().myTransform->Position() + mySettings.myStartPosition;
	mySettings.myEndPosition = GameObject().myTransform->Position() + mySettings.myEndPosition;
}

CMoveActivation::~CMoveActivation()
{
}

void CMoveActivation::OnActivation()
{
	myIsInteracted = true;
}

void CMoveActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();

		Vector3 position;

		if (mySettings.myDuration >= 0.0001f)
		{
			position = Vector3::Lerp(mySettings.myStartPosition, mySettings.myEndPosition, myTime / mySettings.myDuration);
		}
		else
		{
			position = mySettings.myEndPosition;
		}

		GameObject().myTransform->Position(position);

		if (myTime >= mySettings.myDuration)
			Enabled(false);
	}
}