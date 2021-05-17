#include "stdafx.h"
#include "Hierarchy.h"
#include <imgui.h>
#include "ComponentsInclude.h"
#include "Scene.h"
#include "GameObject.h"
#include "Engine.h"


ImGui::CHierarchy::CHierarchy(const char* aName)
	: CWindow(aName)
	, myScene(nullptr)
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

	auto& gameObjects = myScene->ActiveGameObjects();
	int index = 0;
	for (auto& gameObject : gameObjects)
	{
		ImGui::PushID(index);
		bool treeNodeOpen = ImGui::TreeNodeEx(gameObject->Name().c_str(),
			/*ImGuiTreeNodeFlags_DefaultOpen |*/
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth,
			gameObject->Name().c_str());

		if (treeNodeOpen)
		{
			ImGui::Text("ID: %i", gameObject->InstanceID());
			for (auto& component : gameObject->myComponents)
			{
				const auto& type = typeid(*component);
				if (myTypeNames.find(type) == myTypeNames.end())
					SaveClassName(type);

				const char* componentName = myTypeNames[type].c_str();
				bool componentOpen = ImGui::TreeNodeEx(
					componentName,
					ImGuiTreeNodeFlags_FramePadding |
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_SpanAvailWidth,
					componentName);

				if (componentOpen)
				{
					if (myComponentMap.find(type) != myComponentMap.end())
						myComponentMap[type](component.get());
					else
						ImGui::Text("%s WIP", componentName);

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
		index++;
	}
	ImGui::End();
}

void ImGui::CHierarchy::OnDisable()
{
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
	ImGui::Text("%s WIP CPointLightComponent");
}

void ImGui::CHierarchy::Edit(CInstancedModelComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CInstancedModelComponent");
}

void ImGui::CHierarchy::Edit(CCameraControllerComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CCameraControllerComponent");
}

void ImGui::CHierarchy::Edit(CEnvironmentLightComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CEnviromentLightComponent");
}

void ImGui::CHierarchy::Edit(CDecalComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CDecalComponent");
}

void ImGui::CHierarchy::Edit(CRigidBodyComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CRigidBodyComponent");
}

void ImGui::CHierarchy::Edit(CBoxColliderComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CBoxColliderComponent");
}

void ImGui::CHierarchy::Edit(CSphereColliderComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CSphereColliderComponent");
}

void ImGui::CHierarchy::Edit(CCapsuleColliderComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CCapsuleColliderComponent");
}

void ImGui::CHierarchy::Edit(CConvexMeshColliderComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CConvexMeshColliderComponent");
}

void ImGui::CHierarchy::Edit(CVFXSystemComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CVFXSystemComponent");
}

void ImGui::CHierarchy::Edit(CAnimationComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CAnimationComponent");
}

void ImGui::CHierarchy::Edit(CCustomEventComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CCustomEventComponent");
}

void ImGui::CHierarchy::Edit(CCustomEventListenerComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CCustomEventListenerComponent");
}

void ImGui::CHierarchy::Edit(CEnemyComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CEnemyComponent");
}

void ImGui::CHierarchy::Edit(CPlayerComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CPlayerComponent");
}

void ImGui::CHierarchy::Edit(CGravityGloveComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CGravityGloveComponent");
}

void ImGui::CHierarchy::Edit(CHealthPickupComponent* /*aComponent*/)
{
	ImGui::Text("%s WIP CHealthPickupComponent");
}

void ImGui::CHierarchy::Edit(CPlayerControllerComponent* aComponent)
{
	//ImGui::Text("%s WIP CPlayerControllerComponent");

	float walkSpeed = aComponent->WalkSpeed();
	if (DragFloat("Walk Speed", &walkSpeed, 0.001f))
	{
		aComponent->WalkSpeed(walkSpeed);
	}
}
