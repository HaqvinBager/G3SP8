#include "stdafx.h"
#include "EnemyAnimationController.h"

#include "AnimationComponent.h"
#include "EnemyComponent.h"

#define UINT_CAST(a) static_cast<unsigned int>(a)
#define FUNCTION_PRINT std::cout << __FUNCTION__ << std::endl;

CEnemyAnimationController::CEnemyAnimationController()
{}

CEnemyAnimationController::~CEnemyAnimationController()
{
}

void CEnemyAnimationController::Activate()
{
																					// Shape of an R! :D
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemySeekState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyPatrolState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttackState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyIdleState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttack, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyDisabled, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAlertedState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyDetectionState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyUpdateCurrentState, this);
}

void CEnemyAnimationController::Deactivate()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemySeekState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyPatrolState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttackState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyIdleState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttack, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyDisabled, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAlertedState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyDetectionState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyUpdateCurrentState, this);
}

void CEnemyAnimationController::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::EnemyPatrolState:
		{
			CEnemyComponent* enemy = reinterpret_cast<CEnemyComponent*>(aMessage.data);
			OnPatrol(enemy);
		}break;

		case EMessageType::EnemySeekState:
		{
			CEnemyComponent* enemy = reinterpret_cast<CEnemyComponent*>(aMessage.data);
			OnSeek(enemy);
		}break;

		case EMessageType::EnemyAttackState:
		{
			CEnemyComponent* enemy = reinterpret_cast<CEnemyComponent*>(aMessage.data);
			OnAttack(enemy);
		}break;

		case EMessageType::EnemyIdleState:
		{
			CEnemyComponent* enemy = reinterpret_cast<CEnemyComponent*>(aMessage.data);
			OnIdle(enemy);
		}break;

		case EMessageType::EnemyAttack:
		{
			CEnemyComponent* enemy = reinterpret_cast<CEnemyComponent*>(aMessage.data);
			OnAttack(enemy);
		}break;

		case EMessageType::EnemyDisabled:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnDisabled(enemy);
		}break;

		case EMessageType::EnemyAlertedState:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnAlerted(enemy);
		}break;

		case EMessageType::EnemyDetectionState:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnDetected(enemy);
		}break;

		case EMessageType::EnemyUpdateCurrentState:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			UpdateCurrent(enemy);
		}break;

		default:break;
	}
}

void CEnemyAnimationController::OnPatrol(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;
	
	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Walk), UINT_CAST(EEnemyAnimations::Walk), 0.0f);
}

void CEnemyAnimationController::OnSeek(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	float dist = anEnemy->PercentileDistanceToPlayer();
	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Attack), UINT_CAST(EEnemyAnimations::Chase), dist);
}

void CEnemyAnimationController::OnAttack(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendToAnimation(UINT_CAST(EEnemyAnimations::Grab), 0.33f, false, false);
}

void CEnemyAnimationController::OnIdle(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Idle), UINT_CAST(EEnemyAnimations::Idle), 0.0f);
}

void CEnemyAnimationController::OnDisabled(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->Enabled(false);
}

void CEnemyAnimationController::OnAlerted(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Chase), UINT_CAST(EEnemyAnimations::Idle), 1.0f);
}

void CEnemyAnimationController::OnDetected(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Chase), UINT_CAST(EEnemyAnimations::Alert), 1.0f);
}

void CEnemyAnimationController::UpdateCurrent(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	float blend = anEnemy->CurrentStateBlendValue();
	anim->BlendLerp(blend);
}