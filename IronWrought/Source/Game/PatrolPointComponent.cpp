#include "stdafx.h"
#include "PatrolPointComponent.h"

CPatrolPointComponent::CPatrolPointComponent(CGameObject& aParent, float aIntrestValue) 
	: CBehavior(aParent)
	, myBaseIntrestValue(aIntrestValue)
	, myCurrentIntrestValue(myBaseIntrestValue)
	, myBonusIntrestValue(0)
{
}

CPatrolPointComponent::~CPatrolPointComponent()
{
}

float CPatrolPointComponent::GetIntrestValue()
{
	return myCurrentIntrestValue + myBonusIntrestValue;
}

void CPatrolPointComponent::AddBonusValue(float aValue)
{
	//myHasBeenChecked = true;
	myBonusIntrestValue += aValue;
}

void CPatrolPointComponent::AddValue(float aValue)
{
	myCurrentIntrestValue = myBaseIntrestValue + aValue;
}

void CPatrolPointComponent::RemoveValue(float aValue)
{
	myCurrentIntrestValue = myBaseIntrestValue - aValue;
}

void CPatrolPointComponent::Start()
{
}

void CPatrolPointComponent::Awake()
{
}

void CPatrolPointComponent::Update()
{
	if (myBonusIntrestValue > 0) {
		myBonusIntrestValue -= CTimer::Dt();
	}
}

void CPatrolPointComponent::OnEnable()
{
}

void CPatrolPointComponent::OnDisable()
{
}
