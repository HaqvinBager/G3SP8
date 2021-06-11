#pragma once
#include "Behavior.h"
#include "Observer.h"

class CInteractionBehavior : public CBehavior, public IStringObserver
{
public:
	CInteractionBehavior(CGameObject& aParent);
	~CInteractionBehavior() override;

	void OnEnable()	 override;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnDisable() override;

	void Receive(const SStringMessage& aMsg) override;

private:
	void UpdateEyes();

	bool myUpdateEyes;

};

