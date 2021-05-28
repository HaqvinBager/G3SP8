#include "stdafx.h"
#include "ResponseBehavior.h"
#include "ListenerComponent.h"

IResponseBehavior::IResponseBehavior(CGameObject& aParent)
	: CBehavior(aParent)
{
}

void IResponseBehavior::OnEnable()
{
	GetComponent<CListenerComponent>()->Register(this);
}

void IResponseBehavior::OnDisable()
{
	GetComponent<CListenerComponent>()->UnRegister(this);
}
