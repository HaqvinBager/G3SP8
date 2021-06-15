#include "stdafx.h"
#include "PlayVFXActivation.h"
#include <VFXSystemComponent.h>
#include <VFXEffect.h>

CPlayVFXActivation::CPlayVFXActivation(CGameObject& aParent, SSettings someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
	, myVFXSystemComponent(nullptr)
	, myHasVFX(false)
{
}

CPlayVFXActivation::~CPlayVFXActivation()
{
	myVFXSystemComponent = nullptr;
}

void CPlayVFXActivation::Start()
{
	myVFXSystemComponent = GameObject().GetComponent<CVFXSystemComponent>();
	if (myVFXSystemComponent)
		myHasVFX = true;
}

void CPlayVFXActivation::OnActivation()
{
	if (myHasVFX)
	{
		myIsInteracted = true;
		for (auto& effect : myVFXSystemComponent->GetVFXEffects())
			effect.get()->Enable();
	}
}

void CPlayVFXActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();

		if (myTime >= mySettings.myDuration)
		{
			for (auto& effect : myVFXSystemComponent->GetVFXEffects())
				effect.get()->Disable();

			Enabled(false);
		}
	}
}
