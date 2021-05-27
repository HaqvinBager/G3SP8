#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class EResponseType
{
	Rotate,
	Move
};

class CResponseComponent : public CBehavior, public IStringObserver
{
public:
	CResponseComponent(CGameObject& aParent, std::string aReceiveMessage);
	~CResponseComponent();
	
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	virtual void TriggerResponse() = 0;

private:
	void Receive(const SStringMessage& aMessage) override;

	std::string myReceiveMessage;

	bool myHasTriggered;
};

