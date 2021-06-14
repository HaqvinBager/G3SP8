#include "stdafx.h"
#include "EndEventComponent.h"
#include "Scene.h"
#include "PlayerControllerComponent.h"
#include "TransformComponent.h"

CEndEventComponent::CEndEventComponent(CGameObject& aParent, const SEndEventData& aData)
	: IResponseBehavior(aParent)
	, myData(aData)
	, myPathIndex(-1)
	, myTime(0.0f)
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
}

void CEndEventComponent::Update()
{
	if (myPathIndex > -1 && myPathIndex < myData.myEnemyPath.size())
	{
		myTime += CTimer::Dt();
		Vector3 newPosition = Vector3::Lerp(myLastPos, myData.myEnemyPath[myPathIndex].myPosition, myTime / myData.myEnemyPath[myPathIndex].myDuration);
		myEnemy->myTransform->Position(newPosition);

		if (myTime > myData.myEnemyPath[myPathIndex].myDuration)
		{
			myLastPos = myData.myEnemyPath[myPathIndex].myPosition;
			myTime = 0.0f;
			myPathIndex = (myPathIndex + 1) % myData.myEnemyPath.size();

			if (myPathIndex == 0) //PathIndex will only equal Zero when it has played the entire sequence, and would then re-start. We dont want it to restart <3 /Axel Savage 2021-06-14 16:54
			{
				Enabled(false);
			}
		}
	}
}

void CEndEventComponent::OnRespond()
{
	myPlayer->GetComponent<CPlayerControllerComponent>()->LockMovementFor(myData.myLockPlayerDuration);
	std::cout << __FUNCTION__ << " End Event BEGIN" << std::endl;
	myPathIndex = 0;
}