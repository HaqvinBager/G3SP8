#pragma once
#include "Behavior.h"

class IActivationBehavior : public CBehavior
{
public:
	IActivationBehavior(CGameObject& aParent);
	virtual ~IActivationBehavior() override;
	void Awake() override {}
	void Start() override {}
	virtual void Update() override {}
	void OnEnable() override;
	void OnDisable() override;
	void SetHasLock(const bool aHasLock) { myHasLock = aHasLock; }

	bool Complete(const bool aCompletePredicate);

	virtual void OnActivation() 
	{ 
		std::cout << __FUNCTION__ << " Key Activated " << std::endl;
		myIsInteracted = true; 

	};

protected:
	bool myHasLock;
	bool myIsInteracted;
};