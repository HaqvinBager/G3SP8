#pragma once
#include "Behavior.h"
#include "PostMaster.h"

class IResponseBehavior : public CBehavior
{
public:
	IResponseBehavior(CGameObject& aParent);
	virtual ~IResponseBehavior();
	void Awake() override {}
	void Start() override {}
	void OnEnable() override;
	void OnDisable() override; 
	virtual void OnRespond() = 0;
	const std::string ResponseNotify() { return myResponseNotify; }
	const bool HasBeenDelayed() const { return myHasBeenDelayed; }
	void ToggleHasBeenDelayed() { myHasBeenDelayed != myHasBeenDelayed; }
private:
	std::string myResponseNotify;
	bool myHasBeenDelayed;
};

