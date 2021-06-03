#include "stdafx.h"
#include "ActivationBehavior.h"
#include "KeyBehavior.h"

IActivationBehavior::IActivationBehavior(CGameObject& aParent) : CBehavior(aParent)
{
}

IActivationBehavior::~IActivationBehavior()
{
}

void IActivationBehavior::OnEnable()
{
	CKeyBehavior* key = nullptr;
	if (GameObject().TryGetComponent(&key))
		key->Register(this);
}

void IActivationBehavior::OnDisable()
{

}
