#include "stdafx.h"
#include "ActivationBehavior.h"
#include "KeyBehavior.h"

IActivationBehavior::IActivationBehavior(CGameObject& aParent)
	: CBehavior(aParent)
	, myIsInteracted(false)
	, myHasLock(true)
	, myHasBeenDelayed(false)
	, myHasCompleted(false)
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

bool IActivationBehavior::Complete(const bool aCompletePredicate)
{
	myIsInteracted = aCompletePredicate ? false : myIsInteracted;
	myHasCompleted = aCompletePredicate;
	if (aCompletePredicate)
		Enabled(!myHasLock);
	return aCompletePredicate;
}

const bool IActivationBehavior::Complete() const
{
	return myHasCompleted;
}
