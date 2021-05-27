#pragma once
#include "Behavior.h"

class CPatrolPointComponent : public CBehavior
{
public:
	CPatrolPointComponent(CGameObject& aParent, float aIntrestValue);
	~CPatrolPointComponent() override;

	float GetIntrestValue();
	void AddBonusValue(float aValue);
	void AddValue(float aValue);
	void RemoveValue(float aValue);

public:
	void Start() override;
	void Awake() override;
	void Update() override;

public:
	void OnEnable() override;
	void OnDisable() override;
private:
	float myBaseIntrestValue;
	float myBonusIntrestValue;
	float myCurrentIntrestValue;
	bool myHasBeenChecked;
};

