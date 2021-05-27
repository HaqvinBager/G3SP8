#pragma once
#include "Behavior.h"
class IResponseBehavior : public CBehavior
{
public:
	IResponseBehavior(CGameObject& aParent) : CBehavior(aParent) {}
	void Awake() override {}
	void Start() override {}
	void OnEnable() override {}
	void OnDisable() override {}
	virtual void OnRespond() = 0;
private:
};

