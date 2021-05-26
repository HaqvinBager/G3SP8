#pragma once
#include "Behaviour.h"

class CKeyComponent : public CBehaviour
{
public:
	CKeyComponent(CGameObject& aParent, std::string aCreateMessage, std::string aDestroyMessage, void* someData);
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

	std::string myCreateStringMessage;
	std::string myPickUpStringMessage;

	void* myData;
};