#include "stdafx.h"
#include "PlayVFXResponse.h"
#include <VFXSystemComponent.h>
#include <VFXEffect.h>

CPlayVFXResponse::CPlayVFXResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
	, myVFXSystemComponent(nullptr)
	, myHasVFX(false)
{
}

void CPlayVFXResponse::Start()
{
	myVFXSystemComponent = GameObject().GetComponent<CVFXSystemComponent>();
	HasBeenActivated(false);
	if (myVFXSystemComponent)
	{
		myHasVFX = true;
		myVFXSystemComponent->DisableEffect(0);
	}

}

void CPlayVFXResponse::Update()
{
	if (!HasBeenActivated())
		return;

	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		if (mySettings.myDelay >= myTime)
			return;
		ToggleHasBeenDelayed();

		myVFXSystemComponent->EnableEffect(0);

		myTime -= mySettings.myDelay;
	}

	if (myTime >= mySettings.myDuration)
	{
		HasBeenActivated(false);

		myVFXSystemComponent->DisableEffect(0);

		Enabled(false);
	}
}

void CPlayVFXResponse::OnRespond()
{
	if (myHasVFX)
		HasBeenActivated(true);
}
