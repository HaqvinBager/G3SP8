#pragma once
#include "Observer.h"

class CAnimationComponent;
class CEnemyComponent;
class CEnemyAnimationController : public IObserver
{
public:
	enum class EEnemyAnimations
	{
		Alert = 1,
		Attack,
		Chase,// == Seek
		Grab,
		Idle,
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
	void OnIdle(CEnemyComponent* anEnemy);
	void OnDisabled(CEnemyComponent* anEnemy);
	void OnAlerted(CEnemyComponent* anEnemy);
	void UpdateCurrent(CEnemyComponent* anEnemy);
};

