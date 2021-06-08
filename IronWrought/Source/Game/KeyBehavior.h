#pragma once
#include "Behavior.h"

enum class EKeyInteractionTypes
{
	Destroy,
	Rotate,
	Move
};

class IActivationBehavior;

class CKeyBehavior : public CBehavior
{
public:
	struct SSettings
	{
		std::string myOnCreateNotifyName;
		std::string myInteractNotifyName;
		int myOnCreateNotify;
		int myInteractNotify;
		int myHasLock;

		void* myData;
	};

	CKeyBehavior(CGameObject& aParent, const SSettings& someSettings);
	virtual ~CKeyBehavior() override;

public:
	void Awake() override;
	void Start() override;
	virtual void Update() override;

	void Register(IActivationBehavior* aBehaviour);

	void TriggerActivations();

	void OnEnable() override;
	void OnDisable() override;

private:
	SSettings mySettings;
	std::vector<IActivationBehavior*> myActivations;
};