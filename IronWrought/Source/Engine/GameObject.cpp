#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"
#include "Behavior.h"

CGameObject::CGameObject(const int aInstanceID, const std::string& aName, const std::string& aTag)
	: myInstanceID(aInstanceID)
	, myIsStatic(false)
	, myIsActive(true)
	, myName(aName)
	, myTag(aTag)
{
	std::cout << "New GameObject: " << aName << " : Tag: " << myTag << std::endl;
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

	if (aActive)
		for (const auto& component : myComponents)		
			component->Enabled(true);		
	else	
		for (const auto& component : myComponents)	
			component->Enabled(false);		
}

bool CGameObject::CompareTag(const std::string& aTag) const
{
	if (aTag == myTag) {
		return true;
	}
	return false;
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

