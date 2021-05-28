#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class EResponseType
{
	Rotate,
	Move
};

class IResponseBehavior;

class CListenerComponent : public CBehavior, public IStringObserver
{
public:
	CListenerComponent(CGameObject& aParent, std::string aReceiveMessage);
	~CListenerComponent();
	
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void Register(IResponseBehavior* aBehaviour);
	void Deregister(IResponseBehavior* aBehaviour);


	void TriggerResponses();

private:
	void Receive(const SStringMessage& aMessage) override;

	std::string myReceiveMessage;
	std::vector<IResponseBehavior*> myResponses;
	bool myHasTriggered;
};

