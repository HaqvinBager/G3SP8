#include "stdafx.h"
#include "PlayVFXResponse.h"
#include <VFXSystemComponent.h>
#include <VFXEffect.h>

CPlayVFXResponse::CPlayVFXResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
	, myVFXSystemComponent(nullptr)
{
}

void CPlayVFXResponse::Start()
{
	Enabled(true);
	myVFXSystemComponent = GameObject().GetComponent<CVFXSystemComponent>();
	if (!myVFXSystemComponent)
		Enabled(false);
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
		
		for (auto& effect : myVFXSystemComponent->GetVFXEffects())
			effect.get()->Enable();

		myTime -= mySettings.myDelay;
	}

	if (myTime >= mySettings.myDuration)
	{
		HasBeenActivated(false);

		for (auto& effect : myVFXSystemComponent->GetVFXEffects())
			effect.get()->Enable();

		Enabled(false);
	}
}

void CPlayVFXResponse::OnRespond()
{
	HasBeenActivated(true);
}
