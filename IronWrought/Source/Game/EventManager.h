#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include "BinReader.h"

class CGameEvent;

class CEventManager : public CComponent {
public:
	CEventManager(CGameObject& aParent);
	~CEventManager() override;

	void Awake() override { }
	void Start() override { }
	void Update()override { }
	//void InitilizeGameEvent(const Binary::SGameEvent& aEventData);

private:
	std::vector<std::shared_ptr<CGameEvent>> myEvents;
};