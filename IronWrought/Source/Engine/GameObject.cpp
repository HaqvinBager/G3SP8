#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"
#include "Behaviour.h"

CGameObject::CGameObject(const int aInstanceID, const std::string& aName)
	: myInstanceID(aInstanceID)
	, myIsStatic(false)
	, myIsActive(true)
	, myName(aName)
{
	std::cout << "New GameObject: " << aName << std::endl;
	myComponents.push_back(std::make_unique<CTransformComponent>(*this));
	myTransform = static_cast<CTransformComponent*>(myComponents.back().get());
}

CGameObject::~CGameObject()
{
	myTransform = nullptr;
	myComponents.clear();
}

void CGameObject::Awake()
{
	for (const auto& component : myComponents)
		component->Awake();
}

void CGameObject::Start()
{
	for (const auto& component : myComponents)
	{
		if (!component->Enabled())
			continue;
		
		component->Start();
	}
}

void CGameObject::Update()
{
	for (const auto& component : myComponents)
	{
		if (!component->Enabled())
			continue;

		component->Update();
	}
}

void CGameObject::FixedUpdate()
{
	for (const auto& component : myComponents)
	{
		component->FixedUpdate();
	}
}

void CGameObject::LateUpdate()
{
	for (const auto& component : myComponents)
	{
		if (!component->Enabled())
			continue;

		component->LateUpdate();

	}
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		myComponents[i]->LateUpdate();
	}
}

void CGameObject::Active(bool aActive)
{
	myIsActive = aActive;

	if (!aActive)
		for (const auto& component : myComponents)		
			component->OnEnable();		
	else	
		for (const auto& component : myComponents)	
			component->OnDisable();		
}


void CGameObject::Collided(CGameObject& /*aCollidedGameObject*/)
{
	//for (size_t i = 0; i < myComponents.size(); ++i)
	//{
	//	if (CBehaviour* behaviour = dynamic_cast<CBehaviour*>(myComponents[i].get()))
	//	{
	//		if (behaviour->Enabled())
	//		{
	//			myComponents[i]->Collided(&aCollidedGameObject);
	//		}
	//	}
	//	else
	//	{
	//		myComponents[i]->Collided(&aCollidedGameObject);
	//	}
	//}
}

