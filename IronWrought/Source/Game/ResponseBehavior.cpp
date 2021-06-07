#include "stdafx.h"
#include "ResponseBehavior.h"
#include "ListenerBehavior.h"

IResponseBehavior::IResponseBehavior(CGameObject& aParent)
	: CBehavior(aParent), myHasBeenDelayed(false), myHasBeenActivated(true)
{
	GetComponent<CListenerBehavior>()->Register(this);
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
