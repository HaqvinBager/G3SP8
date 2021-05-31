#pragma once
#include "Behavior.h"

class CInteractionBehavior : public CBehavior
{
public:
	CInteractionBehavior(CGameObject& aParent);
	~CInteractionBehavior() override;

	void OnEnable()	 override;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnDisable() override;
private:
	void UpdateEyes();


};

