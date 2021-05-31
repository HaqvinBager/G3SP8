#pragma once
#include "Observer.h"

class CAnimationComponent;
class CEnemyComponent;
class CEnemyAnimationController : public IObserver
{
public:
	enum class EEnemyAnimations
	{
		Attack = 1,
		Chase,
		Idle,
		Search,
		Walk,
		Count
	};

public:
	CEnemyAnimationController();
	~CEnemyAnimationController();

	void Activate();
	void Deactivate();
	void Receive(const SMessage& aMessage) override;

private:
	void OnPatrol(CEnemyComponent* anEnemy);
	void OnSeek(CEnemyComponent* anEnemy);
	void OnAttack(CEnemyComponent* anEnemy);
	void OnTakeDamage(CEnemyComponent* anEnemy);
	void OnDeath(CEnemyComponent* anEnemy);
	void OnDisabled(CEnemyComponent* anEnemy);

private:
	CAnimationComponent* myAnimationComponent;
};

