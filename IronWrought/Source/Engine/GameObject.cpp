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
	//std::cout << "New GameObject: " << aName << " : Tag: " << myTag << std::endl;
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
		if (!component->Enabled())
			continue;

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
	if (myIsActive != aActive)
	{
		for (const auto& component : myComponents)
			component->Enabled(aActive);

		auto& children = myTransform->FetchChildren();
		for (auto& child : children)
		{
			child->GameObject().Active(aActive);
		}
		myIsActive = aActive;
	}
}

bool CGameObject::CompareTag(const std::string& aTag) const
{
	if (aTag == myTag)
	{
		return true;
	}
	return false;
}

const bool CGameObject::HasComponent(const size_t aOtherHashCode) const
{
	for (const auto& component : myComponents)
	{
		const size_t hashCode = typeid(*component.get()).hash_code();
		if (hashCode == aOtherHashCode)
			return true;
	}
	return false;
}

const bool CGameObject::HasComponent(const std::vector<size_t>& someOtherHashCodes) const
{
	for (const auto& component : myComponents)
	{
		const size_t hashCode = typeid(*component.get()).hash_code();
		for (const auto& otherHashCode : someOtherHashCodes)
		{
			if (hashCode == otherHashCode)
				return true;
		}
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

