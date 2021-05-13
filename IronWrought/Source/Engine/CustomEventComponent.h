#pragma once
#include "Component.h"

class CGameObject;
class CCustomEventListenerComponent;
class CCustomEventComponent : public CComponent
{
public:
	CCustomEventComponent(CGameObject& aParent, const char* anEventName);
	~CCustomEventComponent() override;
	void Awake() override;
	void Start() override;
	void Update() override;

	void Register(CCustomEventListenerComponent* aListener);
	void UnRegister(CCustomEventListenerComponent* aListener);

	void OnRaiseEvent();

	const char* Name() const { return myEventName; }
private:
	const char* myEventName;
	std::vector<CCustomEventListenerComponent*> myListeners;
};

