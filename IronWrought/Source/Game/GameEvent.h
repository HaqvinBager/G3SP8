#pragma once
#include "Behavior.h"
#include <vector>

class IListener {
public:
	virtual void OnEventRaised() = 0;
};

class CGameEvent : public CBehavior
{
public:
	CGameEvent(CGameObject& aParent);
	~CGameEvent();

	virtual void OnEnable() {}
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update(){}
	virtual void OnDisable() {}

	void Register(IListener* aListener);
	void UnRegister(IListener* Listener);
	void OnRaiseEvent();
	//const int InstanceID() const { return myInstanceID; }
private:
	int myInstanceID;
	std::vector<IListener*> myListeners;
};