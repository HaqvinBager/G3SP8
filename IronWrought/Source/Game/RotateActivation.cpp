#include "stdafx.h"
#include "RotateActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"

#define PI 3.141592f

CRotateActivation::CRotateActivation(CGameObject& aParent, const SSettings& someSettings) 
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myTime(0.0f)
{
	mySettings.myStartRotation.x = (-mySettings.myStartRotation.x) - 360.0f;
	mySettings.myStartRotation.y += 180.0f;
	mySettings.myStartRotation.z = (-mySettings.myStartRotation.z) - 360.0f;
	mySettings.myStartRotation *= (PI / 180.0f);

	mySettings.myEndRotation.x = (-mySettings.myEndRotation.x) - 360.0f;
	mySettings.myEndRotation.y += 180.0f;
	mySettings.myEndRotation.z = (-mySettings.myEndRotation.z) - 360.0f;
	mySettings.myEndRotation *= (PI / 180.0f);

	myStart = Quaternion::CreateFromYawPitchRoll(mySettings.myStartRotation.y, mySettings.myStartRotation.x, mySettings.myStartRotation.z);
	myEnd =  Quaternion::CreateFromYawPitchRoll(mySettings.myEndRotation.y, mySettings.myEndRotation.x, mySettings.myEndRotation.z);
}

CRotateActivation::~CRotateActivation()
{
}

void CRotateActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();		
		Quaternion rotation = Quaternion::Slerp(myStart, myEnd, myTime / mySettings.myDuration);
		GameObject().myTransform->Rotation(rotation);	
	}

	if (Complete(myTime >= mySettings.myDuration))
	{
		Quaternion temp = myStart;
		myStart = myEnd;
		myEnd = temp;
		myTime -= mySettings.myDuration;
	}
}
