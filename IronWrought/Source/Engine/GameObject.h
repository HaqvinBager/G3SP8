#pragma once
#include <typeinfo>
#include <vector>
#include <memory>
#include <utility>
#include "MainSingleton.h"
#include "PostMaster.h"

class CTransformComponent;
class CComponent;
class CBehaviour;

class CGameObject
{
	friend class CScene;
public:
	CGameObject(const int aInstanceID, const char* aName = "");
	~CGameObject();

	// Inits components. Remember to add CComponents.
	void Awake();
	// Runs on first frame of the game.
	void Start();

	void Update();
	void FixedUpdate();
	void LateUpdate();

	void Collided(CGameObject& aCollidedGameObject);

	void Active(bool aActive);
	bool Active() const { return myIsActive; };

	template< class T, typename... Args >
	T* AddComponent(Args&&... aParams);

	// Removes Component of type. Use typeid( CComponent )
	template<class T>
	void RemoveComponent();

	template<class T>
	T* GetComponent() const;

	template<class T>
	bool TryGetComponent(T** outComponent) const;

	CTransformComponent* myTransform;

	const int InstanceID() const { return  myInstanceID; }

	const bool IsStatic() const { return myIsStatic; }
	void IsStatic(const bool aIsStatic) { myIsStatic = aIsStatic; }

	const char* Name() const { return myName; }

private:
	std::vector<std::unique_ptr<CComponent>> myComponents;
	//std::vector<CComponent*> myComponents;
	bool myIsActive;
	bool myIsStatic;
	const int myInstanceID;
	const char* myName;
};

template<class T, typename... Args >
T* CGameObject::AddComponent(Args&&... aParams)
{
	myComponents.push_back(std::make_unique<T>(std::forward<Args>(aParams)...));
	CMainSingleton::PostMaster().Send({ EMessageType::ComponentAdded, myComponents.back().get() });
	//std::function<void()>([this](CComponent* aComponent) { aComponent->Awake(); }) }
	return dynamic_cast<T*>(myComponents.back().get());
}

//myComponents.emplace_back(std::move(new T(*this, std::forward<Args>(aParams)...)));
//myComponents.back()->myParent = *this;

template<class T>
inline void CGameObject::RemoveComponent()
{
	const std::type_info& type = typeid(T);
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		if (type == typeid(*myComponents[i]))
		{
			std::swap(myComponents[i], myComponents[myComponents.size() - 1]);
			myComponents.pop_back();
		}
	}
}

template<class T>
inline T* CGameObject::GetComponent() const
{
	const std::type_info& type = typeid(T);
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		if (type == typeid(*myComponents[i]))
		{
			return dynamic_cast<T*>(myComponents[i].get());
		}
	}
	//throw std::exception("Component is missing.");
	return nullptr;
}

template<class T>
inline bool CGameObject::TryGetComponent(T** outComponent) const
{
	*outComponent = GetComponent<T>();
	return *outComponent != nullptr;
	//if (component != nullptr) 
	//	outComponent = component;
	//
	//return outComponent != nullptr;
}
