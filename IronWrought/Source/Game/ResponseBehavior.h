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
private:
	std::string myResponseNotify;
};

