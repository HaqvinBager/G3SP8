#pragma once
#include "Behaviour.h"
#include "Observer.h"

class CFuseboxComponent : public CBehaviour, public IObserver
{
public:
	CFuseboxComponent(CGameObject& aParent);
	~CFuseboxComponent() override;

	void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void Receive(const SMessage& aMessage) override;
	void RunEvent();

private:
	int myNumberOfFuses;
	int myNumberOfPickedUpFuses;
	bool myHasTriggered;
};

