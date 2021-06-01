#include "stdafx.h"
#include "ResponseBehavior.h"
#include "ListenerComponent.h"

IResponseBehavior::IResponseBehavior(CGameObject& aParent)
	: CBehavior(aParent), myHasBeenDelayed(false)
{
	GetComponent<CListenerComponent>()->Register(this);
}

IResponseBehavior::~IResponseBehavior()
{
}

void IResponseBehavior::OnEnable()
{
}

void IResponseBehavior::OnDisable()
{
}
