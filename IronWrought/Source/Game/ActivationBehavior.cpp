#include "stdafx.h"
#include "ActivationBehavior.h"

IActivationBehavior::IActivationBehavior(CGameObject& aParent) : CBehavior(aParent)
{
}

IActivationBehavior::~IActivationBehavior()
{
}

void IActivationBehavior::OnEnable()
{
}

void IActivationBehavior::OnDisable()
{
}
