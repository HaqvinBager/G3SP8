#include "stdafx.h"
#include "MoveObjectWithIDResponse.h"
#include "MoveResponse.h"
#include "TransformComponent.h"
#include "Scene.h"

CMoveObjectWithIDResponse::CMoveObjectWithIDResponse(CGameObject& aParent, const SSettings<Vector3>& someSettings, const int& aGOID, const int& aGOIDToCheckIfActive)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
	, myGOIDToMove(aGOID)
	, myGOIDToCheckIfActive(aGOIDToCheckIfActive)
{
	HasBeenActivated(false);
}

void CMoveObjectWithIDResponse::Update()
{
	if (!HasBeenActivated())
		return;

	CGameObject* gameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myGOIDToCheckIfActive);
	if (gameObject)
	{
		if (gameObject->GetComponent<CMoveResponse>())
		{
			gameObject->GetComponent<CMoveResponse>()->Enabled(false);
			//HasBeenActivated(false);
			//return;
		}
	}

	gameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myGOIDToMove);

	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		std::cout << myTime << std::endl;
		if (mySettings.myDelay >= myTime)
			return;

		ToggleHasBeenDelayed();
		myTime -= mySettings.myDelay;
	}

	float t = std::clamp((myTime / mySettings.myDuration), 0.0f, 1.0f);
	gameObject->myTransform->Position(Vector3::Lerp(mySettings.myStart, mySettings.myEnd, t));

	if (myTime >= mySettings.myDuration)
	{
		myTime -= mySettings.myDuration;
		HasBeenActivated(false);
	}
}

void CMoveObjectWithIDResponse::OnRespond()
{
	HasBeenActivated(true);
}