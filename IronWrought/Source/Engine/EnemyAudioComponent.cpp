#include "stdafx.h"
#include "EnemyAudioComponent.h"
#include "EnemyComponent.h"

CEnemyAudioComponent::CEnemyAudioComponent(CGameObject& aParent)
	: CComponent(aParent)
	, myEnemyComponent(nullptr)
	, myStateLastFrame(CEnemyComponent::EBehaviour::Count)
	, myTimer(0.0f)
{
}

CEnemyAudioComponent::~CEnemyAudioComponent()
{
}

void CEnemyAudioComponent::Awake()
{
}

void CEnemyAudioComponent::Start()
{
	myEnemyComponent = GameObject().GetComponent<CEnemyComponent>();
}

void CEnemyAudioComponent::Update()
{
	if (myEnemyComponent->myCurrentState != myStateLastFrame)
	{
		myStateLastFrame = myEnemyComponent->myCurrentState;
		CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyStateChange, &myStateLastFrame });
	}
}
