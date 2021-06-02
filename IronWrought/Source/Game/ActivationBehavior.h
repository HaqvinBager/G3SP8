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
	virtual void OnActivation() = 0;
	//const std::string ResponseNotify() { return myResponseNotify; }

private:
	//std::string myResponseNotify;

};