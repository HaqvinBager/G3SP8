#pragma once
#include "Component.h"

class CGameEvent;
class IListenerComponent : public CComponent {
public:
	IListenerComponent(CGameObject& aParent);
	~IListenerComponent() override;

	void Awake() override {}
	void Start() override {}
	void Update() override {}

	void ConnectToGameEvent(std::weak_ptr<CGameEvent> aGameEvent);

private:
	std::weak_ptr<CGameEvent> myGameEvent;
};
