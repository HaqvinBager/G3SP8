#pragma once
#include "Behavior.h"
#include "PostMaster.h"

class IResponseBehavior : public CBehavior
{
public:
	IResponseBehavior(CGameObject& aParent);
	virtual ~IResponseBehavior() override;
	void Awake() override {}
	void Start() override {}
	virtual void Update() override {}
	void OnEnable() override;
	void OnDisable() override; 
	virtual void OnRespond() = 0;
	const std::string ResponseNotify() { return myResponseNotify; }
	const bool HasBeenDelayed() const { return myHasBeenDelayed; }
	void ToggleHasBeenDelayed() { myHasBeenDelayed = !myHasBeenDelayed; }
	inline const bool HasBeenActivated() const { return myHasBeenActivated; }
	void HasBeenActivated(const bool anActivated) { myHasBeenActivated = anActivated; }
private:
	std::string myResponseNotify;
	bool myHasBeenDelayed;
	bool myHasBeenActivated;
};

