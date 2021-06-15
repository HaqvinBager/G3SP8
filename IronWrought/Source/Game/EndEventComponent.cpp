#include "stdafx.h"
#include "EndEventComponent.h"
#include "Scene.h"
#include "PlayerControllerComponent.h"
#include "TransformComponent.h"
#include <AnimationComponent.h>

CEndEventComponent::CEndEventComponent(CGameObject& aParent, const SEndEventData& aData)
	: IResponseBehavior(aParent)
	, myData(aData)
	, myPathIndex(-1)
	, myTime(0.0f)
	, myLastVingetteStrength(0.0f)
	, myLastAnimationIndex(-1) //5 == Idle
{

}

CEndEventComponent::~CEndEventComponent()
{
}

void CEndEventComponent::Awake()
{
	myPlayer = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myData.myPlayerInstanceID);
	myEnemy = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myData.myEnemyInstanceID);
}

void CEndEventComponent::Start()
{
	myLastPos = GameObject().myTransform->Position();
	myLastRotation = GameObject().myTransform->Rotation();
}

void CEndEventComponent::Update()
{
	if (IsValidPathIndex(myPathIndex))
	{
		myTime += CTimer::Dt();	
		const SPathPoint& point = myData.myEnemyPath[myPathIndex];
		MoveAlongPath(point);
		UpdateAnimation(point);
		UpdateVingette(point);
		UpdatePathIndex(point);
	}
}

void CEndEventComponent::UpdatePathIndex(const SPathPoint& point)
{
	if (myTime > point.myDuration)
	{
		myLastPos = point.myPosition;
		myLastRotation = point.myRotation;
		myLastVingetteStrength = point.myVingetteStrength;
		myLastAnimationIndex = point.myAnimationIndex;
		std::cout << __FUNCTION__ << " Next Index! " << std::endl;
		myTime = 0.0f;
		//std::cout << CTimer::Time() << " /tTime" << std::endl;
		myPathIndex = (myPathIndex + static_cast<int>(1)) % myData.myEnemyPath.size();	
		if (myPathIndex == 0) //PathIndex will only equal Zero when it has played the entire sequence, and would then re-start. We dont want it to restart <3 /Axel Savage 2021-06-14 16:54
		{
			Enabled(false);
		}
	}
}

void CEndEventComponent::OnRespond()
{
	myPlayer->GetComponent<CPlayerControllerComponent>()->LockMovementFor(myData.myLockPlayerDuration);
	std::cout << __FUNCTION__ << " End Event BEGIN" << std::endl;
	myPathIndex = 0;
}

const bool CEndEventComponent::IsValidPathIndex(const int aPathIndex)
{
	return aPathIndex > -1 && aPathIndex < myData.myEnemyPath.size();
}

void CEndEventComponent::MoveAlongPath(const SPathPoint& aPoint)
{
	Vector3 newPosition = Vector3::Lerp(myLastPos, aPoint.myPosition, myTime / aPoint.myDuration);
	Quaternion newRotation = Quaternion::Slerp(myLastRotation, aPoint.myRotation, myTime / aPoint.myDuration);
	myEnemy->myTransform->Rotation(newRotation);
	myEnemy->myTransform->Position(newPosition);
}
void CEndEventComponent::UpdateAnimation(const SPathPoint& aPoint)
{
	if (myLastAnimationIndex != -1)
	{
		if (aPoint.myAnimationIndex != myLastAnimationIndex)
		{
			std::cout << __FUNCTION__ << " Anim Switch to -> " << aPoint.myAnimationIndex << std::endl;
			CAnimationComponent* animator = nullptr;
			if (myEnemy->TryGetComponent(&animator))
			{
				animator->BlendToAnimation(static_cast<unsigned int>(aPoint.myAnimationIndex));
			}
		}
	}
}

void CEndEventComponent::UpdateVingette(const SPathPoint& aPoint)
{
	if (aPoint.myVingetteStrength > 0.01f)
	{
		CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
		myNormalizedBlend = SmoothStep(myLastVingetteStrength, aPoint.myVingetteStrength, myTime / aPoint.myDuration);
		data.myVignetteStrength = Lerp(0.35f, 3.0f, myNormalizedBlend);
		myActualVingetteStrength = data.myVignetteStrength;
		CEngine::GetInstance()->SetPostProcessingBufferData(data);
	}
}