#include "stdafx.h"
#include "MoveResponse.h"
#include "TransformComponent.h"

CMoveResponse::CMoveResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
	Enabled(false);
}

void CMoveResponse::Update()
{
	Vector3 position;
	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		std::cout << myTime << std::endl;
		if (mySettings.myDelay >= myTime)
			return;

		ToggleHasBeenDelayed();
		myTime = 0.0f;
	}

	position = Vector3::Lerp(mySettings.myStartPosition, mySettings.myEndPosition, myTime / mySettings.myDuration);

	GameObject().myTransform->Position(position);

	if (myTime >= mySettings.myDuration)
		Enabled(false);
}

void CMoveResponse::OnRespond()
{
	Enabled(true);
}
