#pragma once
#include "Behavior.h"

class CKeyComponent : public CBehavior
{
public:
	CKeyComponent(CGameObject& aParent, std::string aCreateSendMessage, std::string aPickUpSendMessage, void* someData);
	virtual ~CKeyComponent() override;
	virtual void Destroy();

public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
public:
	virtual void OnPickUp();

	std::string myCreateSendMessage;
	std::string myPickUpSendMessage;

	void* myData;
};