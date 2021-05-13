#pragma once
#include "GameObject.h"
class CComponent
{
	friend class CGameObject;
public:
	CComponent(CGameObject& aParent);
	virtual ~CComponent();

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void FixedUpdate() { };
	virtual void LateUpdate() { };

	virtual void Collided(CGameObject* aCollidedGameObject);

	virtual void OnEnable() {}
	virtual void OnDisable() {}

	virtual void Enabled(bool /*aEnabled*/) { }
	virtual const bool Enabled() const { return true; }

	template<class T>
	T* GetComponent();

	CGameObject& GameObject() const;

private:
	CGameObject& myParent;
};

template<class T>
inline T* CComponent::GetComponent()
{
	return myParent.GetComponent<T>();
}
