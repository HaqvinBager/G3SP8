#include "stdafx.h"
#include "MoveResponse.h"
#include "TransformComponent.h"

CMoveResponse::CMoveResponse(CGameObject& aParent, const SSettings& someSettings) 
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
}

CMoveResponse::~CMoveResponse()
{
}

void CMoveResponse::Update()
{
	Vector3 position;
	myTime += CTimer::Dt();

	position = Vector3::Lerp(mySettings.myStartPosition, mySettings.myEndPosition, myTime / mySettings.myDuration);

	GameObject().myTransform->Position(position);

	if (myTime >= mySettings.myDuration)
		Enabled(false);
}

void CMoveResponse::OnRespond()
{
	Enabled(true);
}
