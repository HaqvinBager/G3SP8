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
	const bool HasBeenDelayed() const { return myHasBeenDelayed; }
	void ToggleHasBeenDelayed() { myHasBeenDelayed = !myHasBeenDelayed; }
	
	/// <summary>
	/// This will let you know if this specefic Activation has reached the
	/// end of its behavior since being activated.
	/// </summary>
	const bool Complete() const;

	virtual void OnActivation() 
	{ 
		std::cout << __FUNCTION__ << " Key Activated " << std::endl;
		myIsInteracted = true; 
	};

protected:
	bool Complete(const bool aCompletePredicate);

	bool myHasCompleted;
	bool myIsInteracted;
	bool myHasLock;
	bool myHasBeenDelayed;
};