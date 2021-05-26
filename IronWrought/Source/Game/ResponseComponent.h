#pragma once
#include <Behaviour.h>
#include <Observer.h>

class CResponseComponent : public CBehaviour, public IStringObserver
{
public:
	CResponseComponent(CGameObject& aParent, std::string aListenStringMessage);
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

	std::string myListenStringMessage;

	bool myHasTriggered;
};

