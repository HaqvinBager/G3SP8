#pragma once
#include "ResponseBehavior.h"
#include "EndEventData.h"

class CPlayerComponent;

class CEndEventComponent : public IResponseBehavior
{
public:
	CEndEventComponent(CGameObject& aParent, const SEndEventData& aData);
	~CEndEventComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void FixedUpdate() override {};
	void LateUpdate() override {}
	void OnRespond() override;

private:
	SEndEventData myData;
	int myPathIndex;
	CGameObject* myPlayer;
	CGameObject* myEnemy;
	float myTime;
	Vector3 myLastPos;


};

