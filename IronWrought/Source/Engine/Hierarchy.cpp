#include "stdafx.h"
#include "Hierarchy.h"
#include <imgui.h>
#include "ComponentsInclude.h"
#include "Scene.h"
#include "GameObject.h"
#include "Engine.h"
#include "JsonReader.h"


ImGui::CHierarchy::CHierarchy(const char* aName)
	: CWindow(aName)
	, myScene(nullptr)
	, myFilteDeepFilterTypes(false)
	, myFilterTypes(false)
	, mySelectedTypeNameIndex(0)
{
	myComponentMap[typeid(CTransformComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CTransformComponent*>(aComponent)); };
	myComponentMap[typeid(CPointLightComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CPointLightComponent*>(aComponent)); };
	myComponentMap[typeid(CCameraComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CCameraComponent*>(aComponent)); };
	myComponentMap[typeid(CModelComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CModelComponent*>(aComponent)); };
	myComponentMap[typeid(CInstancedModelComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CInstancedModelComponent*>(aComponent)); };
	myComponentMap[typeid(CCameraControllerComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CCameraControllerComponent*>(aComponent)); };
	myComponentMap[typeid(CEnvironmentLightComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CEnvironmentLightComponent*>(aComponent)); };
	myComponentMap[typeid(CDecalComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CDecalComponent*>(aComponent)); };
	myComponentMap[typeid(CRigidBodyComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CRigidBodyComponent*>(aComponent)); };
	myComponentMap[typeid(CBoxColliderComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CBoxColliderComponent*>(aComponent)); };
	myComponentMap[typeid(CSphereColliderComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CSphereColliderComponent*>(aComponent)); };
	myComponentMap[typeid(CCapsuleColliderComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CCapsuleColliderComponent*>(aComponent)); };
	myComponentMap[typeid(CConvexMeshColliderComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CConvexMeshColliderComponent*>(aComponent)); };
	myComponentMap[typeid(CVFXSystemComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CVFXSystemComponent*>(aComponent)); };
	myComponentMap[typeid(CAnimationComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CAnimationComponent*>(aComponent)); };
	myComponentMap[typeid(CCustomEventComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CCustomEventComponent*>(aComponent)); };
	myComponentMap[typeid(CCustomEventListenerComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CCustomEventListenerComponent*>(aComponent)); };
	myComponentMap[typeid(CEnemyComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CEnemyComponent*>(aComponent)); };
	myComponentMap[typeid(CPlayerComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CPlayerComponent*>(aComponent)); };
	myComponentMap[typeid(CGravityGloveComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CGravityGloveComponent*>(aComponent)); };
	myComponentMap[typeid(CHealthPickupComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CHealthPickupComponent*>(aComponent)); };
	myComponentMap[typeid(CPlayerControllerComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CPlayerControllerComponent*>(aComponent)); };
		
	myComponentMap[typeid(CLeftClickDownLock)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CLeftClickDownLock*>(aComponent)); };
	myComponentMap[typeid(COnTriggerLock)] = [&](CComponent* aComponent) { Edit(dynamic_cast<COnTriggerLock*>(aComponent)); };
	myComponentMap[typeid(CListenerBehavior)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CListenerBehavior*>(aComponent)); };
	myComponentMap[typeid(CKeyBehavior)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CKeyBehavior*>(aComponent)); };
	myComponentMap[typeid(CEndEventComponent)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CEndEventComponent*>(aComponent)); };
	myComponentMap[typeid(CLightFlickerResponse)] = [&](CComponent* aComponent) { Edit(dynamic_cast<CLightFlickerResponse*>(aComponent)); };

		
	//myCurrentFilter.push_back(typeid(CTransformComponent).hash_code());
	//myCurrentFilter.push_back(typeid(CLeftClickDownLock));
	//myCurrentFilter.push_back(typeid(CPlayerComponent));	
	//myCurrentFilter.push_back(typeid(CListenerBehavior));
	//myCurrentFilter.push_back(typeid(CKeyBehavior));	
	//myCurrentFilter.push_back(typeid(CMoveResponse));
	//myCurrentFilter.push_back(typeid(CMoveActivation));
	//myCurrentFilter.push_back(typeid(CRotateResponse));
	//myCurrentFilter.push_back(typeid(CRotateActivation));
	//myCurrentFilter.push_back(typeid(CToggleResponse));
}

ImGui::CHierarchy::~CHierarchy()
{
}

void ImGui::CHierarchy::OnEnable()
{
	myScene = &CEngine::GetInstance()->GetActiveScene();
}

void ImGui::CHierarchy::OnInspectorGUI()
{
	if (myScene == nullptr)
		return;

	ImGui::Begin(Name(), Open());

	ImGui::BeginHorizontal("Search");
	ImGui::Checkbox("Search Filter:", &mySearchFilterActive);
	ImGui::InputText(" ", mySearchFilter, 256);
	ImGui::EndHorizontal();
	ImGui::BeginHorizontal("Type Filters");
	ImGui::Checkbox("Type Filter", &myFilterTypes);
	ImGui::Checkbox("Deep Type Filter", &myFilteDeepFilterTypes);
	ImGui::EndHorizontal();

	if (ImGui::BeginCombo("Filter By Type", myTypeNames.size() == 0 ? "NoTypes" : myTypeNames[mySelectedTypeNameIndex].c_str()))
	{
		for (int i = 0; i < myTypeNames.size(); ++i)
		{
			if (ImGui::Selectable(myTypeNames[i].c_str(), mySelectedTypeNameIndex == i))
			{
				if (i != mySelectedTypeNameIndex)
				{
					myCurrentFilter.clear();
				}
				mySelectedTypeNameIndex = i;
				myFilterTypes = true;
				myCurrentFilter.push_back(myNameToTypeMap.at(myTypeNames[i]).hash_code());
			}
		}
		ImGui::EndCombo();
	}

	std::vector<CGameObject*> gameObjects = myFilterTypes ? Filter(myScene->ActiveGameObjects(), myCurrentFilter) : myScene->ActiveGameObjects();

	for (const auto& gameObject : gameObjects)
	{
		for (const auto& component : gameObject->myComponents)
		{
			const std::type_index& typeIndex = typeid(*component);
			if (myTypeToNameMap.find(typeIndex) == myTypeToNameMap.end())
				SaveClassName(typeIndex);
		}
	}

	for (auto& gameObject : gameObjects)
	{
		if (mySearchFilterActive)
		{
			if (gameObject->Name().find(mySearchFilter) == std::string::npos)
			{
				continue;
			}
		}

		ImGui::PushID(gameObject->InstanceID());
		bool treeNodeOpen = ImGui::TreeNodeEx(gameObject->Name().c_str(),
			/*ImGuiTreeNodeFlags_DefaultOpen |*/
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth,
			gameObject->Name().c_str());

		if (treeNodeOpen)
		{
			if (gameObject != nullptr)
			{
				ImGui::Text("ID: %i", gameObject->InstanceID());
				for (auto& component : gameObject->myComponents)
				{
					const std::type_index& typeIndex = typeid(*component);

					if (myFilteDeepFilterTypes == true)
					{
						size_t hashCode = typeIndex.hash_code();
						if (std::find(myCurrentFilter.begin(), myCurrentFilter.end(), hashCode) == myCurrentFilter.end())
						{
							continue;
						}		
					}

					if (myTypeToNameMap.find(typeIndex) == myTypeToNameMap.end())
						SaveClassName(typeIndex);

					const char* componentName = myTypeToNameMap[typeIndex].c_str();
					bool componentOpen = ImGui::TreeNodeEx(
						componentName,
						ImGuiTreeNodeFlags_FramePadding |
						ImGuiTreeNodeFlags_OpenOnArrow |
						ImGuiTreeNodeFlags_SpanAvailWidth,
						componentName);

					if (componentOpen)
					{
						bool isEnabled = component->Enabled();
						ImGui::Checkbox("Enabled", &isEnabled);
						if (component->Enabled() != isEnabled)
							component->Enabled(isEnabled);

						if (myComponentMap.find(typeIndex) != myComponentMap.end())
							myComponentMap[typeIndex](component.get());
						else
							ImGui::Text("%s WIP", componentName);

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			else
			{
				ImGui::Text("[ERROR] NULL GameObject");
			}
		}
		ImGui::PopID();
	}
	ImGui::End();
}

void ImGui::CHierarchy::OnDisable()
{
}

void ImGui::CHierarchy::EditGameObjects(std::vector<CGameObject*> someGameObjects)
{

}

std::vector<CGameObject*> ImGui::CHierarchy::Filter(const std::vector<CGameObject*>& someGameObjects, const std::vector<size_t>& filterTypes)
{
	std::vector<CGameObject*> filteredGameObjects = {};
	for (const auto& gameObject : someGameObjects)
	{
		if (gameObject->HasComponent(filterTypes))
			filteredGameObjects.push_back(gameObject);
	}
	return filteredGameObjects;
}

void ImGui::CHierarchy::SubscribeToCallback(const std::type_index& aTypeIndex, std::function<void(CComponent*)> aCallback)
{
	myEditorCallbackMap[aTypeIndex] = aCallback;
}

void ImGui::CHierarchy::Edit(CTransformComponent* aComponent)
{
	Vector3 position = aComponent->Position();
	float pos[3] = { position.x, position.y, position.z };
	if (ImGui::DragFloat3("Position", pos, 0.1f))
	{
		aComponent->Position({ pos[0], pos[1], pos[2] });
	}
}

void ImGui::CHierarchy::Edit(CCameraComponent* aComponent)
{
	float fov = aComponent->GetFoV();
	if (ImGui::DragFloat("Field of View", &fov, 0.5f, 10.0f, 120.0f))
	{
		aComponent->SetFoV(fov);
	}
}

void ImGui::CHierarchy::Edit(CModelComponent* aComponent)
{
	ImGui::Text("Model Path: %s", aComponent->GetModelPath().c_str());
}

void ImGui::CHierarchy::Edit(CPointLightComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CPointLightComponent");
	static int count = 10;

	ImGui::Text("Remaining On/Off Countes: %i", --count);
}

void ImGui::CHierarchy::Edit(CInstancedModelComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CInstancedModelComponent");
}

void ImGui::CHierarchy::Edit(CCameraControllerComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CCameraControllerComponent");
}

void ImGui::CHierarchy::Edit(CEnvironmentLightComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CEnviromentLightComponent");
}

void ImGui::CHierarchy::Edit(CDecalComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CDecalComponent");
}

void ImGui::CHierarchy::Edit(CRigidBodyComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CRigidBodyComponent");
}

void ImGui::CHierarchy::Edit(CBoxColliderComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CBoxColliderComponent");
}

void ImGui::CHierarchy::Edit(CSphereColliderComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CSphereColliderComponent");
}

void ImGui::CHierarchy::Edit(CCapsuleColliderComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CCapsuleColliderComponent");
}

void ImGui::CHierarchy::Edit(CConvexMeshColliderComponent* /*aComponent*/)
{
	ImGui::Text("WIP CConvexMeshColliderComponent");
}

void ImGui::CHierarchy::Edit(CVFXSystemComponent* /*aComponent*/)
{
	ImGui::Text("WIP CVFXSystemComponent");
}

void ImGui::CHierarchy::Edit(CAnimationComponent* /*aComponent*/)
{
	ImGui::Text("WIP CAnimationComponent");
}

void ImGui::CHierarchy::Edit(CCustomEventComponent* aComponent)
{
	if (Button("Raise Event"))
	{
		aComponent->OnRaiseEvent();
	}
}

void ImGui::CHierarchy::Edit(CCustomEventListenerComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CCustomEventListenerComponent");
}

void ImGui::CHierarchy::Edit(CEnemyComponent* aComponent)
{
	aComponent;
#ifdef _DEBUG
	const CEnemyComponent::EBehaviour& currentState = aComponent->myCurrentState;
	ImGui::Text("Current State: %s", aComponent->ToString(currentState).c_str());
	Text("Last Recieved Message %s", aComponent->ToString(aComponent->myLastMessageRecieved).c_str());

	Float("Walk Speed", aComponent->myWalkSpeed);
	Float("Seek Speed", aComponent->mySeekSpeed);
	Float("Current Speed", aComponent->mySettings.mySpeed);

	Float("Aggro Time", aComponent->myAggroTime);
	Float("DeAggro Time", aComponent->myDeAggroTime);
	Float("Aggro Timer", aComponent->myAggroTimer);
	Float("DeAggro Timer", aComponent->myDeAggroTimer);
	Float("Wakup Timer", aComponent->myWakeUpTimer);
	Float("Idling Timer", aComponent->myIdlingTimer);
	Float("Step Timer", aComponent->myStepTimer);
	Float("Attack Time Max", aComponent->myAttackPlayerTimerMax);
	Float("Attack Timer", aComponent->myAttackPlayerTimer);
	
	Checkbox("HasFoundPlayer", &aComponent->myHasFoundPlayer);
	Checkbox("HasReachedLastPlayerPosition", &aComponent->myHasReachedLastPlayerPosition);
	Checkbox("MovementLocked", &aComponent->myMovementLocked);
	Checkbox("HasReachedAlertedTarget", &aComponent->myHasReachedAlertedTarget);
	Checkbox("HeardSound", &aComponent->myHeardSound);
	Checkbox("HasScreamed", &aComponent->myHasScreamed);
	Checkbox("MakesSound", &aComponent->myMakesSound);
#endif
}

void ImGui::CHierarchy::Edit(CPlayerComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CPlayerComponent");
}

void ImGui::CHierarchy::Edit(CGravityGloveComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CGravityGloveComponent");
}

void ImGui::CHierarchy::Edit(CHealthPickupComponent* /*aComponent*/)
{
	ImGui::Text(" WIP CHealthPickupComponent");
}

void ImGui::CHierarchy::Edit(CPlayerControllerComponent* aComponent)
{
	//ImGui::Text(" WIP CPlayerControllerComponent");

	float walkSpeed = aComponent->WalkSpeed();
	if (DragFloat("Walk Speed", &walkSpeed, 0.001f))
	{
		aComponent->WalkSpeed(walkSpeed);
	}
}

void ImGui::CHierarchy::Edit(CLeftClickDownLock* aComponent)
{
	ImGui::Checkbox("Is Triggered", &aComponent->myHasTriggered);
	if (ImGui::Button("Raise Event"))
	{
		aComponent->RunEventEditor();
	}
}

void ImGui::CHierarchy::Edit(COnTriggerLock* aComponent)
{
	ImGui::Checkbox("Is Triggered", &aComponent->myHasTriggered);
	if (ImGui::Button("Raise Event"))
	{
		aComponent->RunEventEditor();
	}
}

void ImGui::CHierarchy::Edit(CListenerBehavior* aComponent)
{
	//aComponent;
	//ImGui::Text(" WIP CListenerBehavior");
	const int id = aComponent->RecieveMessage();
	ImGui::Text("Recieve Message: %i", id);
	std::string messageName = {};
	if (CJsonReader::Get()->TryGetAssetPath(id, messageName))
	{
		ImGui::Text("Name: %s", messageName.c_str());
	}

}

void ImGui::CHierarchy::Edit(CKeyBehavior* aComponent)
{
	aComponent;
	ImGui::Text(" WIP CKeyBehavior");
}

void ImGui::CHierarchy::Edit(CEndEventComponent* aComponent)
{
	ImGui::Text("Path Point");
	const SPathPoint& point = aComponent->CurrentPoint();
	ImGui::Text("Duration %f", point.myDuration);
	ImGui::Text("Vingette Strength: %f", point.myVingetteStrength);

	ImGui::Text("Actual Vingette: %f", aComponent->ActualVingetteStrength());
	ImGui::Text("Vingette Normalized Blend: %f", aComponent->VingetteNormalizedBlend());
}

void ImGui::CHierarchy::Edit(CLightFlickerResponse* aComponent)
{
	float noise = aComponent->Noise();
	ImGui::DragFloat("Noise Value", &noise, 0.01f, -1.0f, 1.0f);
}

void ImGui::CHierarchy::Float(const std::string& aText, float aValue)
{
	std::string txt = aText;

	size_t length = aText.length();
	if (length < 20)
	{
		for (size_t i = 0; i < 20 - length; ++i)
		{
			txt.append(" ");
		}
	}
	txt.append(" %f");
	ImGui::Text(txt.c_str(), aValue);
}

	//static bool plState = false;

	//ImGui::Checkbox("Test PL", &plState);
	//if (plState)
	//{
	//	std::vector<CPointLightComponent*> pointLights = {};
	//	if (myScene->TryGetAllComponents(pointLights))
	//	{
	//		gameObjects.clear();

	//		for (auto& pl : pointLights)
	//		{
	//			gameObjects.push_back(&pl->GameObject());
	//		}
	//	}

	//	if (ImGui::Button("Test Add Pl"))
	//	{
	//		auto player = myScene->FindFirstObjectWithComponent<CPlayerControllerComponent>();
	//		auto testPl = player->GameObject().AddComponent<CPointLightComponent>(player->GameObject(), 300.0f, Vector3(0.0f, 0.0f, 1.0f), 100.0f);
	//		myScene->AddInstance(testPl->GetPointLight());
	//	}
	//}