#include "stdafx.h"
#include "MoveResponse.h"
#include "TransformComponent.h"

CMoveResponse::CMoveResponse(CGameObject& aParent, const SSettings<Vector3>& someSettings)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
	HasBeenActivated(false);
}

void CMoveResponse::Update()
{
	if (!HasBeenActivated())
		return;
	
	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		std::cout << myTime << std::endl;
		if (mySettings.myDelay >= myTime)
			return;

		ToggleHasBeenDelayed();
		myTime -= mySettings.myDelay;
	}

	GameObject().myTransform->Position(Vector3::Lerp(mySettings.myStart, mySettings.myEnd, myTime / mySettings.myDuration));

	if (myTime >= mySettings.myDuration)
	{
		myTime -= mySettings.myDuration;
		HasBeenActivated(false);
	}
}

void CMoveResponse::OnRespond()
{
	HasBeenActivated(true);
}