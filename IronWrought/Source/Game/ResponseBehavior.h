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

private:
	std::string myResponseNotify;
};

