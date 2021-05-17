#pragma once
#include "Behaviour.h"

class CGameObject;
class CCustomEventComponent;

class CCustomEventListenerComponent : public CBehaviour
{
public:
	CCustomEventListenerComponent(CGameObject& aParent, CCustomEventComponent* aCustomEvent);
	~CCustomEventListenerComponent() override;
		
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
	
	void SetResponse(std::function<void()> aResponse);


	virtual void OnCustomEventRaised();

private:
	CCustomEventComponent* myCustomEvent;
	std::function<void()> myResponse;
};

