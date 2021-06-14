#include "stdafx.h"
#include "RotateActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"

#define PI 3.141592f

CRotateActivation::CRotateActivation(CGameObject& aParent, const SSettings<Quaternion>& someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
{
}

CRotateActivation::~CRotateActivation()
{
}

void CRotateActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();		
		float t = std::clamp((myTime / mySettings.myDuration), 0.0f, 1.0f);
		Quaternion rotation = Quaternion::Slerp(mySettings.myStart, mySettings.myEnd, t);
		GameObject().myTransform->Rotation(rotation);	
	}

	if (Complete(myTime >= mySettings.myDuration))
	{
		Quaternion temp = mySettings.myStart;
		mySettings.myStart = mySettings.myEnd;
		mySettings.myEnd = temp;
		myTime -= mySettings.myDuration;
	}
}
