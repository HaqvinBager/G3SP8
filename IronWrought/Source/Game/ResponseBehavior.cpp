#include "stdafx.h"
#include "ResponseBehavior.h"
#include "ListenerComponent.h"

IResponseBehavior::IResponseBehavior(CGameObject& aParent)
	: CBehavior(aParent)
{
	GetComponent<CListenerComponent>()->Register(this);
}

IResponseBehavior::~IResponseBehavior()
{
	GetComponent<CListenerComponent>()->Deregister(this);
}

void IResponseBehavior::OnEnable()
{
}

void IResponseBehavior::OnDisable()
{
}
