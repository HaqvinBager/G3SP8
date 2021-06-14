#include "stdafx.h"
#include "PlayVFXActivation.h"
#include <VFXSystemComponent.h>
#include <VFXEffect.h>

CPlayVFXActivation::CPlayVFXActivation(CGameObject& aParent, SSettings someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
	, myVFXSystemComponent(nullptr)
{
}

CPlayVFXActivation::~CPlayVFXActivation()
{
}

void CPlayVFXActivation::Start()
{
	const auto& vfxComponent = GameObject().GetComponent<CVFXSystemComponent>();
	if (!vfxComponent)
		Enabled(false);
}

void CPlayVFXActivation::OnActivation()
{
	myIsInteracted = true;
	for (auto& effect : myVFXSystemComponent->GetVFXEffects())
		effect.get()->Enable();
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
