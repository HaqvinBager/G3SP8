#pragma once
#include "Window.h"
#include <functional>
#include <typeindex>

class CScene;
class CComponent;

class CCameraComponent;
class CInstancedModelComponent;
class CCameraControllerComponent;
class CEnvironmentLightComponent;
class CModelComponent;
class CPointLightComponent;
class CDecalComponent;
class CRigidBodyComponent;
class CBoxColliderComponent;
class CSphereColliderComponent;
class CCapsuleColliderComponent;
class CConvexMeshColliderComponent;
class CVFXSystemComponent;
class CAnimationComponent;
class CCustomEventComponent;
class CCustomEventListenerComponent;
class CEnemyComponent;
class CPlayerComponent;
class CGravityGloveComponent;
class CHealthPickupComponent;
class CPlayerControllerComponent;
class CLeftClickDownLock;
class COnTriggerLock;
class CListenerBehavior;
class CKeyBehavior;
class CMoveResponse;
class CRotateResponse;
class CMoveActivation;
class CMoveResponse;
class CToggleResponse;

namespace ImGui {
	class CHierarchy : public CWindow
	{
	public:

		CHierarchy(const char* aName);
		~CHierarchy() override;

		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;

		

		void EditGameObjects(std::vector<CGameObject*> someGameObjects);
		std::vector<CGameObject*> Filter(const std::vector<CGameObject*>& someGameObjects, const std::vector<size_t>& filterTypes);

		void SubscribeToCallback(const std::type_index& aTypeIndex, std::function<void(CComponent*)> aCallback);

		void Edit(CPointLightComponent*			aComponent);
		void Edit(CTransformComponent*			aComponent);
		void Edit(CCameraComponent*				aComponent);
		void Edit(CModelComponent*				aComponent);
		void Edit(CInstancedModelComponent*		aComponent);
		void Edit(CCameraControllerComponent*	aComponent);
		void Edit(CEnvironmentLightComponent*	aComponent);
		void Edit(CDecalComponent*				aComponent);
		void Edit(CRigidBodyComponent*			aComponent);
		void Edit(CBoxColliderComponent*		aComponent);
		void Edit(CSphereColliderComponent*		aComponent);
		void Edit(CCapsuleColliderComponent*	aComponent);
		void Edit(CConvexMeshColliderComponent* aComponent);
		void Edit(CVFXSystemComponent*			aComponent);
		void Edit(CAnimationComponent*			aComponent);
		void Edit(CCustomEventComponent*		aComponent);
		void Edit(CCustomEventListenerComponent* aComponent);
		void Edit(CEnemyComponent*				aComponent);
		void Edit(CPlayerComponent*				aComponent);
		void Edit(CGravityGloveComponent*		aComponent);
		void Edit(CHealthPickupComponent*		aComponent);
		void Edit(CPlayerControllerComponent*	aComponent);
		void Edit(CLeftClickDownLock* aComponent);
		void Edit(COnTriggerLock* aComponent);
		void Edit(CListenerBehavior* aComponent);
		void Edit(CKeyBehavior* aComponent);

	private:
		std::unordered_map<std::type_index, std::function<void(CComponent*)>> myComponentMap;
		std::unordered_map<std::type_index, std::function<void(CComponent*)>> myEditorCallbackMap;
		
		std::unordered_map<std::type_index, std::string> myTypeToNameMap;
		std::unordered_map<std::string, const std::type_index> myNameToTypeMap;
		std::vector<std::string> myTypeNames;

		bool mySearchFilterActive;
		bool myFilterTypes;
		bool myFilteDeepFilterTypes;
		char mySearchFilter[256];

		int mySelectedTypeNameIndex;
		CScene* myScene;
		std::vector<size_t> myCurrentFilter;

	private:
		void SaveClassName(const std::type_index& aType)
		{
			std::string name = aType.name();
			size_t startIndex = 6; //class *space* (6 chars)
			size_t count = name.size() - startIndex;
			name = name.substr(startIndex, count);
			myTypeToNameMap[aType] = name;
			myNameToTypeMap.insert({ name, aType });
			myTypeNames.push_back(name);
		}
	};
}

