#include "stdafx.h"
#include "EndEventComponent.h"
#include "Scene.h"
#include "PlayerControllerComponent.h"
#include "TransformComponent.h"
#include <AnimationComponent.h>
#include "CameraComponent.h"

CEndEventComponent::CEndEventComponent(CGameObject& aParent, const SEndEventData& aData)
	: IResponseBehavior(aParent)
	, myData(aData)
	, myPathIndex(-1)
	, myTime(0.0f)
	, myVignetteTime(0.0f)
	, myHalfTime(0.0f)
	, myLastVignetteStrength(0.0f)
	, myLastAnimationIndex(-1) //5 == Idle
	, myEventHasCompleted(false)
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

	for (size_t i = 0; i < myData.myEnemyPath.size(); ++i)
	{
		myHalfTime += myData.myEnemyPath[i].myDuration;
	}
	myHalfTime *= 0.4f;
}

void CEndEventComponent::Update()
{
	if (myEventHasCompleted)
	{
		myTime += CTimer::Dt();

		if (myHalfTime < myTime)
		{

			OnEndEventComplete();
			return;
		}
	}
	else
	{
		if (IsValidPathIndex(myPathIndex))
		{
			myTime += CTimer::Dt();
			if (myPathIndex > 1)
			{
				myVignetteTime += CTimer::Dt();
			}
			const SPathPoint& point = myData.myEnemyPath[myPathIndex];
			MoveAlongPath(point);
			UpdateAnimation(point);
			UpdateVignette(point);
			UpdatePathIndex(point);
		}
	}
}

void CEndEventComponent::UpdatePathIndex(const SPathPoint& point)
{
	if (myTime > point.myDuration)
	{
		myLastPos = point.myPosition;
		myLastRotation = point.myRotation;
		//myLastVingetteStrength = point.myVingetteStrength;
		myLastAnimationIndex = point.myAnimationIndex;
		myTime = 0.0f;
		myPathIndex = (myPathIndex + static_cast<int>(1)) % myData.myEnemyPath.size();


		if (myPathIndex > (myData.myEnemyPath.size() - 3))
		{
			myData.myEnemyPath[myPathIndex].myPosition = { myPlayer->myTransform->Position().x, myData.myEnemyPath[myPathIndex].myPosition.y, myPlayer->myTransform->Position().z };
		}

		//if (myPathIndex == myData.myEnemyPath.size() - 1)
		//{
		//	StartPostEvent();
		//}

		if (myPathIndex == 0)
		{
			myTime = 0.0f;
			myHalfTime = 3.0f;
			myEventHasCompleted = true;
			//Enabled(false);
		}
	}
}

void CEndEventComponent::StartPostEvent()
{
	CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().MainCamera();
	camera->Fade(false, 2.5f, true);

	Vector3 playerPos = myPlayer->myTransform->Position();
	Vector3 enemyPos = myEnemy->myTransform->Position();

	enemyPos.x = playerPos.x;
	enemyPos.z = playerPos.z;

	//myData.myEnemyPath[myPathIndex].myPosition = enemyPos;
}

void CEndEventComponent::OnEndEventComplete()
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::EndOfGameEvent, nullptr });
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

void CEndEventComponent::UpdateVignette(const SPathPoint& /*aPoint*/)
{
	static bool hasStartedPostEvent = false;
	static float vignetteStrength = 0.0f;


	CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
	if (myVignetteTime <= myHalfTime)
	{
		vignetteStrength = myVignetteTime / myHalfTime;
		//myNormalizedBlend = SmoothStep(myLastVignetteStrength, vignetteStrength, vignetteStrength);
		data.myVignetteStrength = Lerp(0.35f, 7.0f, vignetteStrength);

		myLastVignetteStrength = data.myVignetteStrength;
		std::cout << __FUNCTION__ << "Lerping\n" << "Vignette strength: " << data.myVignetteStrength << std::endl;
	}
	else
	{
		vignetteStrength += CTimer::Dt();
		data.myVignetteStrength = 7.0f;
		std::cout << __FUNCTION__ << "Set strength\n" << "Vignette strength: " << data.myVignetteStrength << std::endl;
		if (!hasStartedPostEvent)
		{
			StartPostEvent();
			hasStartedPostEvent = true;
		}
	}
	//myActualVingetteStrength = data.myVignetteStrength;
	CEngine::GetInstance()->SetPostProcessingBufferData(data);

		IRONWROUGHT_ACTIVE_SCENE.MainCamera()->SetTrauma(vignetteStrength, CCameraComponent::ECameraShakeState::EnemySway);
}