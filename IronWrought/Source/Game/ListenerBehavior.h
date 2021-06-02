#pragma once
#include "Behavior.h"
#include "Observer.h"

enum class EResponseType
{
	Rotate,
	Move
};

class IResponseBehavior;

class CListenerBehavior : public CBehavior, public IStringObserver
{
public:
	CListenerBehavior(CGameObject& aParent, std::string aReceiveMessage);
	~CListenerBehavior();
	
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void Register(IResponseBehavior* aBehaviour);

	void TriggerResponses();

private:
	void Receive(const SStringMessage& aMessage) override;

	std::string myReceiveMessage;
	std::vector<IResponseBehavior*> myResponses;
	bool myHasTriggered;
};

