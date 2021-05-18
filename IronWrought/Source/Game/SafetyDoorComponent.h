#pragma once
#include "Behaviour.h"
#include "Observer.h"

class CSafetyDoorComponent : public CBehaviour, public IObserver
{
public:
	CSafetyDoorComponent(CGameObject& aParent);
	~CSafetyDoorComponent() override;

	void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void Receive(const SMessage& aMessage) override;
private:
	bool myShouldOpenDoors;
	bool myIsOpen;
	float myOpenDoorPosition;
};

