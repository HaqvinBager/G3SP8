#include "stdafx.h"
#include "SceneManager.h"
#include "EngineDefines.h"
#include "EnvironmentLight.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "ModelComponent.h"

#include "PointLightComponent.h"
#include "DecalComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidBodyComponent.h"
#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "ConvexMeshColliderComponent.h"
#include "VFXSystemComponent.h"
#include "PhysicsPropAudioComponent.h"
#include <GravityGloveComponent.h>
#include <EnemyComponent.h>
#include <HealthPickupComponent.h>
#include <PatrolPointComponent.h>
#include <InteractionBehavior.h>

#include "NavmeshLoader.h"
#include <BinReader.h>
#include <PlayerControllerComponent.h>
#include <AIController.h>
#include <PlayerComponent.h>
#include "animationLoader.h"
#include "AnimationComponent.h"
#include "TeleporterComponent.h"

#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include "CustomEventComponent.h"
#include "CustomEventListenerComponent.h"
#include "SpotLightComponent.h"

#include <LockBehavior.h>
#include <OnTriggerLock.h>
#include <LeftClickDownLock.h>
#include <KeyBehavior.h>
#include <DestroyKey.h>
#include <AnimateKey.h>
#include <ListenerComponent.h>
#include <MoveResponse.h>
#include <RotateResponse.h>
#include <PrintResponse.h>
#include <ToggleResponse.h>

CScene* CSceneManager::ourLastInstantiatedScene = nullptr;
CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
}

CScene* CSceneManager::CreateEmpty()
{
	CScene* emptyScene = Instantiate();

	CGameObject* camera = new CGameObject(0);
	camera->AddComponent<CCameraComponent>(*camera);//Default Fov is 70.0f
	camera->AddComponent<CCameraControllerComponent>(*camera); //Default speed is 2.0f
	camera->myTransform->Position({ 0.0f, 10.0f, 0.0f });

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnvironmentLightComponent>(*envLight, "Cubemap_Inside");
	envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight()->SetIntensity(0.f);
	envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight()->SetColor({ 0.f, 0.f, 0.f });
	envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight()->SetDirection({ 0.0f,1.0f,1.0f });

	emptyScene->AddInstance(camera);
	emptyScene->AddCamera(camera->GetComponent<CCameraComponent>(), ESceneCamera::FreeCam);
	emptyScene->MainCamera(ESceneCamera::FreeCam);
	emptyScene->EnvironmentLight(envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight());
	emptyScene->AddInstance(envLight);



	//AddPlayer(*emptyScene, std::string());

	return emptyScene;
}

CScene* CSceneManager::CreateScene(const std::string& aSceneName)
{
	CScene* scene = Instantiate();
	//CScene* scene = CreateEmpty();

	const auto doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aSceneName + "/" + aSceneName + ".json"));
	if (doc.HasParseError())
		return nullptr;

	if (!doc.HasMember("Root"))
		return nullptr;

	Binary::SLevelData binLevelData = CBinReader::Load(ASSETPATH("Assets/Generated/" + aSceneName + "/" + aSceneName + ".bin"));
	
	AddToScene(*scene, binLevelData, doc);
	return scene;
}

CScene* CSceneManager::CreateSceneFromSeveral(const std::vector<std::string>& someSceneNames)
{
	CScene* scene = Instantiate(static_cast<int>(someSceneNames.size()));
	for (int i = 0; i < someSceneNames.size(); ++i)
	{
		const std::string& sceneName = someSceneNames[i];
		const auto doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + sceneName + "/" + sceneName + ".json"));
		if (doc.HasParseError())
		{
			std::cout << __FUNCTION__ << " Scene: " << sceneName << " has parse errors! Unable to load." << std::endl;
			scene->NextSection();
			continue;
		}
		if (!doc.HasMember("Root"))
		{
			std::cout << __FUNCTION__ << " Scene: " << sceneName << " has no root! Unable to load." << std::endl;
			scene->NextSection();
			continue;
		}

		Binary::SLevelData binLevelData = CBinReader::Load(ASSETPATH("Assets/Generated/" + sceneName + "/" + sceneName + ".bin"));

		AddToScene(*scene, binLevelData, doc, i);
		scene->NextSection();
	}
	scene->SetCurrentSection(0);

	return scene;
}

CScene* CSceneManager::CreateMenuScene(const std::string& aCanvasPath)
{
	CScene* scene = CreateEmpty();

	assert(scene != nullptr && "Scene is nullptr. This shouldn't happen.");
	scene->MainCamera()->GameObject().GetComponent<CCameraControllerComponent>()->SetCameraMode(CCameraControllerComponent::ECameraMode::MenuCam);
	if (aCanvasPath.empty())
	{
		//assert(false && "aCanvasPath parameter is empty.");
		return scene;
	}
	scene->InitCanvas(aCanvasPath);

	return scene;
}

CScene* CSceneManager::Instantiate(const int aNumberOfSections)
{
	// Depending on the order in which we Awake() different states/ scenes are created some states/scenes that should not listen to ComponentAdded might do so and will cause issues. / Aki 2021 04 08
	if (ourLastInstantiatedScene != nullptr)
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::ComponentAdded, ourLastInstantiatedScene);

	ourLastInstantiatedScene = new CScene(aNumberOfSections); //Creates a New scene and Leaves total ownership of the Previous scene over to the hands of Engine!

	//Create Cameras

	CMainSingleton::PostMaster().Subscribe(EMessageType::ComponentAdded, ourLastInstantiatedScene);
	return ourLastInstantiatedScene;
}

bool CSceneManager::AddToScene(CScene& aScene, Binary::SLevelData& aBinLevelData, const rapidjson::Document& aDoc, const int aSceneSection)
{
	SVertexPaintCollection vertexPaintData = CBinReader::LoadVertexPaintCollection(aDoc["Root"].GetString());
	const auto& scenes = aDoc.GetObjectW()["Scenes"].GetArray();

	if (AddGameObjects(aScene, aBinLevelData.myInstanceIDs))
	{
		SetTransforms(aScene, aBinLevelData.myTransforms);

		AddInstancedModelComponents(aScene, aBinLevelData.myInstancedModels);
		AddPointLights(aScene, aBinLevelData.myPointLights);
		AddModelComponents(aScene, aBinLevelData.myModels);
		AddCollider(aScene, aBinLevelData.myColliders);
		AddSpotLights(aScene, aBinLevelData.mySpotLights);

		if (aDoc.HasMember("NavMeshData"))
		{
			if (aDoc.GetObjectW()["NavMeshData"].HasMember("path"))
			{
				std::string navMeshPath = aDoc.GetObjectW()["NavMeshData"].GetObjectW()["path"].GetString();
				if (!navMeshPath.empty())
				{
					std::cout << __FUNCTION__ << " navmesh found: " << navMeshPath << "\n";
					aScene.InitNavMesh(ASSETPATH(navMeshPath));
				}
				else
					std::cout << __FUNCTION__ << " navmesh path is empty!\n";
			}else
				std::cout << __FUNCTION__ << " level does not contain path to navmesh!\n";
		}else
			std::cout << __FUNCTION__ << " navmesh not found!\n";

		//CreateCustomEvents(aScene);
		//CreateCustomEventListeners(aScene);

		for (const auto& sceneData : scenes)
		{
			std::string sceneName = sceneData["sceneName"].GetString();

			if (sceneData.HasMember("parents"))
				SetParents(aScene, sceneData["parents"].GetArray());

			if (sceneData.HasMember("locks"))
				AddPuzzleLock(aScene, sceneData["locks"].GetArray());
			if (sceneData.HasMember("keys"))
				AddPuzzleKey(aScene, sceneData["keys"].GetArray());
			if (sceneData.HasMember("listeners"))
				AddPuzzleListener(aScene, sceneData["listeners"].GetArray());
			if (sceneData.HasMember("moves"))
				AddPuzzleResponseMove(aScene, sceneData["moves"].GetArray());
			if (sceneData.HasMember("rotates"))
				AddPuzzleResponseRotate(aScene, sceneData["rotates"].GetArray());
			if (sceneData.HasMember("prints"))
				AddPuzzleResponsePrint(aScene, sceneData["prints"].GetArray());
			if (sceneData.HasMember("toggles"))
				AddPuzzleToggle(aScene, sceneData["toggles"].GetArray());

			AddDirectionalLights(aScene, sceneData["directionalLights"].GetArray());
			SetVertexPaintedColors(aScene, sceneData["vertexColors"].GetArray(), vertexPaintData);
			AddDecalComponents(aScene, sceneData["decals"].GetArray());
			AddPickups(aScene, sceneData["healthPickups"].GetArray());
			AddAudioSources(aScene, sceneData["myAudioSources"].GetArray());
			if (sceneData.HasMember("myVFXLinks"))
				AddVFX(aScene, sceneData["myVFXLinks"].GetArray());

			if (sceneData.HasMember("triggerEvents"))
				AddTriggerEvents(aScene, sceneData["triggerEvents"].GetArray(), aSceneSection);
			if (sceneData.HasMember("teleporters"))
				AddTeleporters(aScene, sceneData["teleporters"].GetArray());

			if (sceneName.find("Gameplay") != std::string::npos)//Om Unity Scene Namnet inneh�ller nyckelordet "Layout"
				AddPlayer(aScene, sceneData["player"].GetObjectW());

			if (sceneData.HasMember("enemies"))
				AddEnemyComponents(aScene, sceneData["enemies"].GetArray());
		}
	}

	//scene->NavMesh();
	//if (AddGameObjects(aScene, sceneData["Ids"].GetArray()))
	//{
	//	SetTransforms(aScene, sceneData["transforms"].GetArray());

	//	AddDirectionalLights(aScene, sceneData["directionalLights"].GetArray());
	//	AddPointLights(aScene, sceneData["lights"].GetArray());
	//	AddModelComponents(aScene, sceneData["models"].GetArray());
	//	SetVertexPaintedColors(aScene, sceneData["vertexColors"].GetArray(), vertexPaintData);
	//	AddDecalComponents(aScene, sceneData["decals"].GetArray());
	//	AddCollider(aScene, sceneData["colliders"].GetArray());
	//	if (sceneData.HasMember("triggerEvents"))
	//		AddTriggerEvents(aScene, sceneData["triggerEvents"].GetArray());

	//	if (sceneName.find("Layout") != std::string::npos)//Om Unity Scene Namnet inneh�ller nyckelordet "Layout"
	//	{
	//		AddPlayer(aScene, sceneData["player"].GetObjectW());
	//	}
	//	AddEnemyComponents(aScene, sceneData["enemies"].GetArray());

	//	if(sceneData.HasMember("healthPickups"))
	//		AddPickups(aScene, sceneData["healthPickups"].GetArray());
	//}
	//AddInstancedModelComponents(aScene, sceneData["instancedModels"].GetArray());
	//}

	CEngine::GetInstance()->GetPhysx().Cooking(aScene.ActiveGameObjects(), &aScene);
	return true;
}

bool CSceneManager::AddGameObjects(CScene& aScene, RapidArray someData)
{
	if (someData.Size() == 0)
		return false;

	for (const auto& jsonID : someData)
	{
		int instanceID = jsonID.GetInt();
		aScene.AddInstance(new CGameObject(instanceID));
	}
	return true;
}

bool CSceneManager::AddGameObjects(CScene& aScene, const std::vector<Binary::SInstanceID>& someData)
{
	if (someData.size() == 0)
		return false;

	for (int i = 0; i < someData.size(); ++i)
	{
		aScene.AddInstance(new CGameObject(someData[i].instanceID, someData[i].name, someData[i].tag));
	}
	return true;
}

void CSceneManager::SetTransforms(CScene& aScene, RapidArray someData)
{
	for (const auto& t : someData)
	{
		int id = t["instanceID"].GetInt();
		CTransformComponent* transform = aScene.FindObjectWithID(id)->myTransform;
		transform->Scale({ t["scale"]["x"].GetFloat(),
							  t["scale"]["y"].GetFloat(),
							  t["scale"]["z"].GetFloat() });
		transform->Rotation({ t["rotation"]["x"].GetFloat(),
							  t["rotation"]["y"].GetFloat(),
							  t["rotation"]["z"].GetFloat() });
		transform->Position({ t["position"]["x"].GetFloat(),
							  t["position"]["y"].GetFloat(),
							  t["position"]["z"].GetFloat() });
	}
}

void CSceneManager::SetTransforms(CScene& aScene, const std::vector<Binary::STransform>& someData)
{
	for (const auto& t : someData)
	{
		CTransformComponent* transform = aScene.FindObjectWithID(t.instanceID)->myTransform;
		transform->Scale(t.scale);
		transform->Rotation(t.rot);
		transform->Position(t.pos);
	}
}

void CSceneManager::CreateCustomEvents(CScene& aScene)
{
	CGameObject* gameObject = new CGameObject(200, "Add");
	gameObject->AddComponent<CCustomEventComponent>(*gameObject, "Add");
	aScene.AddInstance(gameObject);
}

void CSceneManager::CreateCustomEventListeners(CScene& aScene)
{
	CCustomEventComponent* customEvent = aScene.FindObjectWithID(200)->GetComponent<CCustomEventComponent>();
	CGameObject* gameObject = new CGameObject(201, "Listener");
	gameObject->AddComponent<CCustomEventListenerComponent>(*gameObject, customEvent);
	//float aRange, DirectX::SimpleMath::Vector3 aColorAndIntensity, float anIntensity
	gameObject->AddComponent<CPointLightComponent>(*gameObject, 100.0f, Vector3(0.0f, 0.0f, 1.0f), 250.0f);
	aScene.AddInstance(gameObject->GetComponent<CPointLightComponent>()->GetPointLight());
	aScene.AddInstance(gameObject);
}

void CSceneManager::SetParents(CScene& aScene, RapidArray someData)
{
	for (const auto& parent : someData)
	{
		int parentInstanceID = parent["parent"]["instanceID"].GetInt();
		CTransformComponent* parentTransform = aScene.FindObjectWithID(parentInstanceID)->myTransform;

		for (const auto& child : parent["children"].GetArray())
		{
			int childInstanceID = child["instanceID"].GetInt();
			CTransformComponent* childTransform = aScene.FindObjectWithID(childInstanceID)->myTransform;
			childTransform->SetParent(parentTransform);
		}
	}
}

//void CSceneManager::AddModelComponents(CScene& aScene, RapidArray someData)
//{
//	for (const auto& m : someData)
//	{
//		const int instanceId = m["instanceID"].GetInt();
//		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
//		if (!gameObject)
//			continue;
//
//		const int assetId = m["assetID"].GetInt();
//		if (CJsonReader::Get()->HasAssetPath(assetId))
//		{
//			std::string assetPath = ASSETPATH(CJsonReader::Get()->GetAssetPath(assetId));
//			gameObject->AddComponent<CModelComponent>(*gameObject, assetPath);
//			AnimationLoader::AddAnimationsToGameObject(gameObject, assetPath);// Does nothing if the Model has no animations.
//		}
//	}
//}

void CSceneManager::AddModelComponents(CScene& aScene, const std::vector<Binary::SModel>& someData)
{
	for (const auto& m : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(m.instanceID);
		if (gameObject == nullptr)
			continue;

		gameObject->AddComponent<CModelComponent>(*gameObject, m);
		AnimationLoader::AddAnimationsToGameObject(gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(m.assetID)));
		//if (AnimationLoader::AddAnimationsToGameObject(gameObject, CJsonReader::Get()->GetAssetPath(m.assetID)))
		//{
		//	std::cout << __FUNCTION__ << " Animation " << CJsonReader::Get()->GetAssetPath(m.assetID) << std::endl;
		//}
		//else
		//{
		//	std::cout << __FUNCTION__ << " No animation " << CJsonReader::Get()->GetAssetPath(m.assetID) << std::endl;
		//}
	}
}

void CSceneManager::SetVertexPaintedColors(CScene& aScene, RapidArray someData, const SVertexPaintCollection& vertexColorData)
{
	for (const auto& i : someData)
	{
		std::vector<std::string> materials;
		materials.reserve(3);
		for (const auto& material : i["materialNames"].GetArray())
		{
			materials.push_back(material.GetString());
		}

		int vertexColorID = i["vertexColorsID"].GetInt();
		for (const auto& gameObjectID : i["instanceIDs"].GetArray())
		{
			CGameObject* gameObject = aScene.FindObjectWithID(gameObjectID.GetInt());
			if (gameObject == nullptr) //Was not entirely sure why this would try to acces a non-existant gameObject.
				continue;				//I think it relates to the Model or VertexPaint Export. We don't want to add vertexColors to the export if said object was never painted on!

			for (auto it = vertexColorData.myData.begin(); it != vertexColorData.myData.end(); ++it)
			{
				if ((*it).myVertexMeshID == vertexColorID)
				{
					gameObject->GetComponent<CModelComponent>()->InitVertexPaint(it, materials);
				}
			}
		}
	}
}

//void CSceneManager::AddInstancedModelComponents(CScene& aScene, RapidArray someData)
//{
//	for (const auto& i : someData)
//	{
//		int assetID = i["assetID"].GetInt();
//		CGameObject* gameObject = new CGameObject(assetID);
//		gameObject->IsStatic(true);
//		std::vector<Matrix> instancedModelTransforms;
//		instancedModelTransforms.reserve(i["transforms"].GetArray().Size());
//
//		for (const auto& t : i["transforms"].GetArray())
//		{
//			CGameObject temp(0);
//			CTransformComponent transform(temp);
//			transform.Scale({ t["scale"]["x"].GetFloat(),
//								 t["scale"]["y"].GetFloat(),
//								 t["scale"]["z"].GetFloat() });
//			transform.Position({ t["position"]["x"].GetFloat(),
//								 t["position"]["y"].GetFloat(),
//								 t["position"]["z"].GetFloat() });
//			transform.Rotation({ t["rotation"]["x"].GetFloat(),
//								 t["rotation"]["y"].GetFloat(),
//								 t["rotation"]["z"].GetFloat() });
//			instancedModelTransforms.emplace_back(transform.GetLocalMatrix());
//		}
//		if (CJsonReader::Get()->HasAssetPath(assetID))
//		{
//			gameObject->AddComponent<CInstancedModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(assetID)), instancedModelTransforms);
//			aScene.AddInstance(gameObject);
//		}
//	}
//}

void CSceneManager::AddInstancedModelComponents(CScene& aScene, const std::vector<Binary::SInstancedModel>& someData)
{
	for (const auto& i : someData)
	{
		CGameObject* gameObject = new CGameObject(i.assetID, "", i.tag);


		gameObject->AddComponent<CInstancedModelComponent>(*gameObject, i);
		aScene.AddInstance(gameObject);
		//std::string assetPath = {};
		//if (CJsonReader::Get()->TryGetAssetPath(i.assetID, assetPath))
		//{
			//gameObject->AddComponent<CInstancedModelComponent>(*gameObject, ASSETPATH(assetPath), i.materialIDs, transforms);
		//}
	}
}

void CSceneManager::AddDirectionalLight(CScene& aScene, RapidObject someData)
{
	const auto& id = someData["instanceID"].GetInt();

	if (id == 0)
		return;

	CGameObject* gameObject = aScene.FindObjectWithID(id);
	if (gameObject == nullptr)
		return;

	gameObject->AddComponent<CEnvironmentLightComponent>(
		*gameObject,
		someData["cubemapName"].GetString(),
		Vector3(someData["r"].GetFloat(),
			someData["g"].GetFloat(),
			someData["b"].GetFloat()),
		someData["intensity"].GetFloat(),
		Vector3(someData["direction"]["x"].GetFloat(),
			someData["direction"]["y"].GetFloat(),
			someData["direction"]["z"].GetFloat())
		);

	if (someData["isMainDirectionalLight"].GetBool())
		aScene.EnvironmentLight(gameObject->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight());
}

void CSceneManager::AddDirectionalLights(CScene& aScene, RapidArray someData)
{
	for (const auto& directionalLight : someData)
	{
		const auto& id = directionalLight["instanceID"].GetInt();

		if (id == 0)
			continue;

		CGameObject* gameObject = aScene.FindObjectWithID(id);
		if (gameObject == nullptr)
			continue;

		gameObject->AddComponent<CEnvironmentLightComponent>(
			*gameObject,
			directionalLight["cubemapName"].GetString(),
			Vector3(directionalLight["r"].GetFloat(),
				directionalLight["g"].GetFloat(),
				directionalLight["b"].GetFloat()),
			directionalLight["intensity"].GetFloat(),
			Vector3(directionalLight["direction"]["x"].GetFloat(),
				directionalLight["direction"]["y"].GetFloat(),
				directionalLight["direction"]["z"].GetFloat())
			);

		auto light = gameObject->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight();
		light->SetIsVolumetric(directionalLight["isVolumetric"].GetBool());
		light->SetIsFog(directionalLight["isFog"].GetBool());
		light->SetNumberOfSamples(directionalLight["numberOfSamples"].GetFloat());
		light->SetLightPower(directionalLight["lightPower"].GetFloat());
		light->SetScatteringProbability(directionalLight["scatteringProbability"].GetFloat());
		light->SetHenyeyGreensteinGValue(directionalLight["henyeyGreensteinGValue"].GetFloat());
		light->SetPosition(gameObject->myTransform->Position());

		if (directionalLight["isMainDirectionalLight"].GetBool())
			aScene.EnvironmentLight(gameObject->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight());
		else
			aScene.AddInstance(gameObject->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight());
	}
}

void CSceneManager::AddPointLights(CScene& aScene, RapidArray someData)
{
	for (const auto& pointLight : someData)
	{
		const auto& id = pointLight["instanceID"].GetInt();

		CGameObject* gameObject = aScene.FindObjectWithID(id);
		if (gameObject == nullptr)
			continue;

		CPointLightComponent* pointLightComponent = gameObject->AddComponent<CPointLightComponent>(
			*gameObject,
			pointLight["range"].GetFloat(),
			Vector3(pointLight["r"].GetFloat(),
				pointLight["g"].GetFloat(),
				pointLight["b"].GetFloat()),
			pointLight["intensity"].GetFloat());
		aScene.AddInstance(pointLightComponent->GetPointLight());
	}
}

void CSceneManager::AddPointLights(CScene& aScene, const std::vector<Binary::SPointLight>& someData)
{
	for (const auto& pointLight : someData)
	{
		const auto& id = pointLight.instanceID;

		CGameObject* gameObject = aScene.FindObjectWithID(id);
		if (gameObject == nullptr)
			continue;

		CPointLightComponent* pointLightComponent = gameObject->AddComponent<CPointLightComponent>(
			*gameObject,
			pointLight.range,
			pointLight.color,
			pointLight.intensity);
		aScene.AddInstance(pointLightComponent->GetPointLight());
	}
}

void CSceneManager::AddSpotLights(CScene& aScene, const std::vector<Binary::SSpotLight>& someData)
{
	for (const auto& data : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(data.instanceID);

		if (gameObject == nullptr)
			continue;

		auto component = gameObject->AddComponent<CSpotLightComponent>(*gameObject, data);
		aScene.AddInstance(component->GetSpotLight());
	}
}

void CSceneManager::AddPuzzleKey(CScene& aScene, RapidArray someData)
{
	for (const auto& key : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(key["instanceID"].GetInt());
		gameObject;
		CKeyBehavior::SSettings settings = { key["onCreateNotify"].GetString(), key["onInteractNotify"].GetString(), nullptr };
		EKeyInteractionTypes interactionType = static_cast<EKeyInteractionTypes>(key["interactionType"].GetInt());
		switch (interactionType)
		{
		case EKeyInteractionTypes::Destroy:
		{
			gameObject->AddComponent<CDestroyKey>(*gameObject, settings);
		}
		break;
		case EKeyInteractionTypes::Animate:
		{
			CAnimateKey::SAnimateKeySettings animateKeySettings =
			{
				{
					0.0f, 0.0f, 0.0f
				},
				{
					0.0f, 0.0f, 0.0f
				},
				{
					0.0f, 0.0f, 135.0f
				},
				{
					0.0f, 0.0f, 45.0f
				},
				0.5f
				//{	
				//	key["startPosition"]["x"].GetFloat(),
				//	key["startPosition"]["y"].GetFloat(),
				//	key["startPosition"]["z"].GetFloat() 
				//},
				//{	
				//	key["endPosition"]["x"].GetFloat(),
				//	key["endPosition"]["y"].GetFloat(),
				//	key["endPosition"]["z"].GetFloat() 
				//},
				//{	
				//	key["startRotation"]["x"].GetFloat(),
				//	key["startRotation"]["y"].GetFloat(),
				//	key["startRotation"]["z"].GetFloat() 
				//},
				//{	
				//	key["endRotation"]["x"].GetFloat(),
				//	key["endRotation"]["y"].GetFloat(),
				//	key["endRotation"]["z"].GetFloat() 
				//},
				//key["duration"].GetFloat()
			};
			gameObject->AddComponent<CAnimateKey>(*gameObject, settings, animateKeySettings);
		}
		break;
		default:
			break;
		}
	}
}

void CSceneManager::AddPuzzleLock(CScene& aScene, RapidArray someData)
{
	for (const auto& lock : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(lock["instanceID"].GetInt());
		gameObject;
		CLockBehavior::SSettings settings = { lock["onKeyCreateNotify"].GetString(), lock["onKeyInteractNotify"].GetString(), lock["onNotify"].GetString(), nullptr};
		ELockInteractionTypes interactionType = static_cast<ELockInteractionTypes>(lock["interactionType"].GetInt());
		switch (interactionType)
		{
		case ELockInteractionTypes::OnTriggerEnter:
		{
			gameObject->AddComponent<COnTriggerLock>(*gameObject, settings);
		}
		break;
		case ELockInteractionTypes::OnLeftClickDown:
		{
			gameObject->AddComponent<CLeftClickDownLock>(*gameObject, settings);
		}
		break;
		default:
			break;
		}
	}
}

void CSceneManager::AddPuzzleListener(CScene& aScene, RapidArray someData)
{
	for (const auto& listener : someData)
	{
		std::string onResponseNotify = listener["onResponseNotify"].GetString();
		CGameObject* gameObject = aScene.FindObjectWithID(listener["instanceID"].GetInt());

		gameObject->AddComponent<CListenerComponent>(*gameObject, onResponseNotify);
	}
}

void CSceneManager::AddPuzzleResponseMove(CScene& aScene, RapidArray someData)
{
	for (const auto& response : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(response["instanceID"].GetInt());
		if (!gameObject)
			continue;

		CMoveResponse::SSettings settings = {};
		settings.myDuration = response["duration"].GetFloat();
		settings.myDelay = response["delay"].GetFloat();

		settings.myStartPosition = { response["start"]["x"].GetFloat(),
									 response["start"]["y"].GetFloat(),
									 response["start"]["z"].GetFloat() };

		settings.myEndPosition = {  response["end"]["x"].GetFloat(),
									response["end"]["y"].GetFloat(),
									response["end"]["z"].GetFloat() };

		gameObject->AddComponent<CMoveResponse>(*gameObject, settings);
	}
}

void CSceneManager::AddPuzzleResponseRotate(CScene& aScene, RapidArray someData)
{
	for (const auto& response : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(response["instanceID"].GetInt());
		if (!gameObject)
			continue;

		CRotateResponse::SSettings settings = {};
		settings.myDuration = response["duration"].GetFloat();
		settings.myDelay = response["delay"].GetFloat();

		settings.myStartRotation = { response["start"]["x"].GetFloat(),
									 response["start"]["y"].GetFloat(),
									 response["start"]["z"].GetFloat() };

		settings.myEndRotation = { response["end"]["x"].GetFloat(),
									response["end"]["y"].GetFloat(),
									response["end"]["z"].GetFloat() };

		gameObject->AddComponent<CRotateResponse>(*gameObject, settings);
	}
}

void CSceneManager::AddPuzzleResponsePrint(CScene& aScene, RapidArray someData)
{
	for (const auto& response : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(response["instanceID"].GetInt());
		if (!gameObject)
			continue;

		CPrintResponse::SSettings settings = {};
		settings.myData = response["data"].GetString();

		gameObject->AddComponent<CPrintResponse>(*gameObject, settings);
	}
}

void CSceneManager::AddPuzzleToggle(CScene& aScene, RapidArray someData)
{
	for (const auto& response : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(response["instanceID"].GetInt());
		if (!gameObject)
			continue;

		CToggleResponse::SSettings settings = {};
		settings.myType = response["type"].GetString();
		settings.myEnableOnStart = response["enableOnStartup"].GetInt() ? 1 : 0;
		settings.myEnableOnNotify = response["enableOnNotify"].GetInt() ? 1 : 0;
		settings.myTargetInstanceID = response["target"].GetInt();

		gameObject->AddComponent<CToggleResponse>(*gameObject, settings);
	}
}

void CSceneManager::AddDecalComponents(CScene& aScene, RapidArray someData)
{
	for (const auto& decal : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(decal["instanceID"].GetInt());
		gameObject->AddComponent<CDecalComponent>(*gameObject, decal["materialName"].GetString());
	}
}

void CSceneManager::AddPlayer(CScene& aScene, RapidObject someData)
{
	if (aScene.PlayerController())
		return;

	int instanceID = someData["instanceID"].GetInt();
	CGameObject* player = aScene.FindObjectWithID(instanceID);

	Quaternion playerRot = player->myTransform->Rotation();

	CGameObject* camera = CCameraControllerComponent::CreatePlayerFirstPersonCamera(player);//new CGameObject(1000);
	camera->AddComponent<CInteractionBehavior>(*camera);

	camera->myTransform->Rotation(playerRot);
	//std::string modelPath = ASSETPATH("Assets/IronWrought/Mesh/Main_Character/CH_PL_SK.fbx");
	//camera->AddComponent<CModelComponent>(*camera, modelPath);
	//AnimationLoader::AddAnimationsToGameObject(camera, modelPath);
	CGameObject* gravityGloveSlot = new CGameObject(PLAYER_GLOVE_ID);
	gravityGloveSlot->myTransform->Scale(0.1f);
	gravityGloveSlot->myTransform->SetParent(camera->myTransform);
	gravityGloveSlot->myTransform->Position({ 0.f, 0.f, 1.5f });
	gravityGloveSlot->myTransform->Rotation(playerRot);

	camera->AddComponent<CGravityGloveComponent>(*camera, gravityGloveSlot->myTransform);
	player->AddComponent<CPlayerComponent>(*player);
	float speedModifider = 0.7f;
	float walkSpeed = 0.09f * speedModifider;
	CPlayerControllerComponent* pcc = player->AddComponent<CPlayerControllerComponent>(*player, walkSpeed, walkSpeed * 0.4f, CEngine::GetInstance()->GetPhysx().GetPlayerReportBack());// CPlayerControllerComponent constructor sets position of camera child object.
	pcc->SprintSpeedModifier(speedModifider * 2.0f);
	pcc->StepTime((walkSpeed / speedModifider) * (4.0f / speedModifider));// Short explanation: for SP7 Nico added a steptimer for playback of stepsounds. It was set to walkSpeed * 5.0f. Changing walk speed to something lower does not give desirable results (shorter timer for slower speed sounds odd). Hence this.
	//camera->AddComponent<CVFXSystemComponent>(*camera, ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystem_Player.json"));

	//aScene.AddInstance(model);
	aScene.AddInstance(camera);
	aScene.AddInstance(gravityGloveSlot);
	aScene.AddCamera(camera->GetComponent<CCameraComponent>(), ESceneCamera::PlayerFirstPerson);
	aScene.MainCamera(ESceneCamera::PlayerFirstPerson);
	aScene.Player(player);
}

void CSceneManager::AddEnemyComponents(CScene& aScene, RapidArray someData)
{
	for (const auto& m : someData)
	{
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		SEnemySetting settings;
		settings.myRadius = m["radius"].GetFloat();
		settings.mySpeed = m["speed"].GetFloat();
		//settings.myHealth = m["health"].GetFloat();
		settings.myAttackDistance = m["attackDistance"].GetFloat();
		if (m.HasMember("interestPoints"))
		{
			for (const auto& point : m["interestPoints"].GetArray())
			{
				settings.myPatrolGameObjectIds.emplace_back(point["transform"]["instanceID"].GetInt());
				settings.myPatrolIntrestValue.emplace_back(point["interestValue"].GetFloat());

				CGameObject* patrolPoint = aScene.FindObjectWithID(point["transform"]["instanceID"].GetInt());
				auto patrolComponent = patrolPoint->AddComponent<CPatrolPointComponent>(*patrolPoint, point["interestValue"].GetFloat());
				aScene.AddInstance(patrolComponent);
			}
		}
		gameObject->AddComponent<CEnemyComponent>(*gameObject, settings);
		gameObject->GetComponent<CAnimationComponent>()->BlendLerpBetween(1, 2, 0.2f);
		//gameObject->AddComponent<CModelComponent>(*gameObject, ASSETPATH("Assets/IronWrought/Mesh/Enemy/CH_Enemy_SK.fbx"));
		//AnimationLoader::AddAnimationsToGameObject(gameObject, ASSETPATH("Assets/IronWrought/Mesh/Enemy/CH_Enemy_SK.fbx"));

		//gameObject->AddComponent<CPatrolPointComponent>(*gameObject, )
		//gameObject->AddComponent<CVFXSystemComponent>(*gameObject, ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystem_Enemy.json"));
	}
}

void CSceneManager::AddPickups(CScene& aScene, RapidArray someData)
{
	for (const auto& m : someData)
	{
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		float healthPickupAmount = m["healthPickupAmount"].GetFloat();
		gameObject->AddComponent<CHealthPickupComponent>(*gameObject, healthPickupAmount);
	}
}

void CSceneManager::AddAudioSources(CScene& aScene, RapidArray someData)
{
	for (const auto& m : someData)
	{
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		if (m["is3D"].GetBool())
		{
			PostMaster::SStaticAudioSourceInitData data =
			{
				  gameObject->myTransform->Position()
				, { m["coneDirection"]["x"].GetFloat(), m["coneDirection"]["y"].GetFloat(), m["coneDirection"]["z"].GetFloat() }
				, m["minAttenuationAngle"].GetFloat()
				, m["maxAttenuationAngle"].GetFloat()
				, m["minimumVolume"].GetFloat()
				, m["soundIndex"].GetInt()
				, instanceId
			};
			CMainSingleton::PostMaster().Send({ EMessageType::AddStaticAudioSource, &data });
		}
		else
		{
			gameObject->AddComponent<CPhysicsPropAudioComponent>(*gameObject, static_cast<unsigned int>(m["soundIndex"].GetInt()));
		}
	}
}

void CSceneManager::AddVFX(CScene& aScene, RapidArray someData)
{
	for (const auto& m : someData)
	{
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		std::string jsonPath = "Assets/IronWrought/VFX/VFX_JSON/VFXSystem_";
		auto component = gameObject->AddComponent<CVFXSystemComponent>(*gameObject, ASSETPATH(jsonPath + m["effectName"].GetString() + ".json"));

		component->EnableEffect(0); // Temp
	}
}

void CSceneManager::AddCollider(CScene& aScene, RapidArray someData)
{
	//const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));
	//if (!CJsonReader::IsValid(doc, { "colliders" }))
	//	return;

	//const auto& colliders = doc.GetObjectW()["colliders"].GetArray();
	for (const auto& c : someData)
	{
		int id = c["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(id);

		ColliderType colliderType = static_cast<ColliderType>(c["colliderType"].GetInt());
		bool isStatic = c.HasMember("isStatic") ? c["isStatic"].GetBool() : false;
		bool isKinematic = c.HasMember("isKinematic") ? c["isKinematic"].GetBool() : false;
		bool isTrigger = c.HasMember("isTrigger") ? c["isTrigger"].GetBool() : false;
		unsigned int layer = c.HasMember("layer") ? c["layer"].GetInt() : 1;


		CRigidBodyComponent* rigidBody = gameObject->GetComponent<CRigidBodyComponent>();
		if (rigidBody == nullptr && isStatic == false)
		{
			float mass = c["mass"].GetFloat();
			Vector3 localCenterMass;
			localCenterMass.x = c["localMassPosition"]["x"].GetFloat();
			localCenterMass.y = c["localMassPosition"]["y"].GetFloat();
			localCenterMass.z = c["localMassPosition"]["z"].GetFloat();
			Vector3 inertiaTensor;
			inertiaTensor.x = c["inertiaTensor"]["x"].GetFloat();
			inertiaTensor.y = c["inertiaTensor"]["y"].GetFloat();
			inertiaTensor.z = c["inertiaTensor"]["z"].GetFloat();
			gameObject->AddComponent<CRigidBodyComponent>(*gameObject, mass, localCenterMass, inertiaTensor, isKinematic);
		}

		Vector3 posOffset;
		posOffset.x = c["positionOffest"]["x"].GetFloat();
		posOffset.y = c["positionOffest"]["y"].GetFloat();
		posOffset.z = c["positionOffest"]["z"].GetFloat();

		float dynamicFriction = c["dynamicFriction"].GetFloat();
		float staticFriction = c["staticFriction"].GetFloat();
		float bounciness = c["bounciness"].GetFloat();

		switch (colliderType)
		{
		case ColliderType::BoxCollider:
		{
			Vector3 boxSize;
			boxSize.x = c["boxSize"]["x"].GetFloat();
			boxSize.y = c["boxSize"]["y"].GetFloat();
			boxSize.z = c["boxSize"]["z"].GetFloat();
			gameObject->AddComponent<CBoxColliderComponent>(*gameObject, posOffset, boxSize, isTrigger, layer, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
		}
		break;
		case ColliderType::SphereCollider:
		{
			float radius = c["sphereRadius"].GetFloat();
			gameObject->AddComponent<CSphereColliderComponent>(*gameObject, posOffset, radius, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
		}
		break;
		case ColliderType::CapsuleCollider:
		{
			float radius = c["capsuleRadius"].GetFloat();
			float height = c["capsuleHeight"].GetFloat();
			gameObject->AddComponent<CCapsuleColliderComponent>(*gameObject, posOffset, radius, height, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
		}
		break;
		case ColliderType::MeshCollider:
		{
			gameObject->AddComponent<CConvexMeshColliderComponent>(*gameObject, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
		}
		break;
		}
	}
}

void CSceneManager::AddCollider(CScene& aScene, const std::vector<Binary::SCollider>& someData)
{
	for (const auto& c : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(c.instanceID);
		ColliderType colliderType = static_cast<ColliderType>(c.colliderType);
		CRigidBodyComponent* rigidBody = gameObject->GetComponent<CRigidBodyComponent>();
		
		if (rigidBody == nullptr && c.isStatic == false)
		{
			gameObject->AddComponent<CRigidBodyComponent>(*gameObject, c.mass, c.localMassPosition, c.inertiaTensor, c.isKinematic);
		}

		switch (colliderType)
		{
		case ColliderType::BoxCollider:
		{
			gameObject->AddComponent<CBoxColliderComponent>(*gameObject, c.positionOffest, c.boxSize, c.isTrigger, c.layer, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(c.dynamicFriction, c.staticFriction, c.bounciness));
		}
		break;
		case ColliderType::SphereCollider:
		{
			gameObject->AddComponent<CSphereColliderComponent>(*gameObject, c.positionOffest, c.sphereRadius, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(c.dynamicFriction, c.staticFriction, c.bounciness));
		}
		break;
		case ColliderType::CapsuleCollider:
		{
			gameObject->AddComponent<CCapsuleColliderComponent>(*gameObject, c.positionOffest, c.capsuleRadius, c.capsuleHeight, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(c.dynamicFriction, c.staticFriction, c.bounciness));
		}
		break;
		case ColliderType::MeshCollider:
		{
			gameObject->AddComponent<CConvexMeshColliderComponent>(*gameObject, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(c.dynamicFriction, c.staticFriction, c.bounciness));
		}
		break;
		}
	}
}

void CSceneManager::AddTriggerEvents(CScene& aScene, RapidArray someData, const int aSection)
{
	for (const auto& triggerEvent : someData)
	{
		int instanceID = triggerEvent["instanceID"]["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceID);

		CBoxColliderComponent* triggerVolume = nullptr;
		if (gameObject->TryGetComponent<CBoxColliderComponent>(&triggerVolume))
		{
			std::string eventData = triggerEvent["gameEvent"].GetString();
			int eventFilter = triggerEvent.HasMember("eventFilter") ? triggerEvent["eventFilter"].GetInt() : static_cast<int>(CBoxColliderComponent::EEventFilter::Any);
			int audioIndex = triggerEvent.HasMember("audioIndex") ? triggerEvent["audioIndex"].GetInt() : -1;
			bool triggerOnce = triggerEvent.HasMember("triggerOnce") ? triggerEvent["triggerOnce"].GetBool() : false;
			triggerVolume->RegisterEventTriggerMessage(eventData);
			triggerVolume->RegisterEventTriggerFilter(eventFilter);
			triggerVolume->RegisterEventTriggerAudioIndex(audioIndex);
			triggerVolume->RegisterEventTriggerOnce(triggerOnce);
			triggerVolume->RegisterSceneSection(aSection);
		}
	}
}

void CSceneManager::AddTeleporters(CScene& aScene, const RapidArray& someData)
{
	for (const auto& teleporter : someData)
	{
		int instanceID = teleporter["instanceID"]["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceID);

		CBoxColliderComponent* triggerVolume = nullptr;
		if (gameObject->TryGetComponent<CBoxColliderComponent>(&triggerVolume))
		{
			triggerVolume->RegisterEventTriggerMessage(PostMaster::SMSG_TELEPORT);
			triggerVolume->RegisterEventTriggerFilter(static_cast<int>(CBoxColliderComponent::EEventFilter::PlayerOnly));
			triggerVolume->RegisterEventTriggerAudioIndex(-1);
			triggerVolume->RegisterEventTriggerOnce(false);

			CTeleporterComponent::ELevelName teleportersName = static_cast<CTeleporterComponent::ELevelName>(teleporter["myTeleporterName"].GetInt());
			CTeleporterComponent::ELevelName teleportTo = static_cast<CTeleporterComponent::ELevelName>(teleporter["teleportTo"].GetInt());
			Vector3 position = { teleporter["teleportObjectToX"].GetFloat(), teleporter["teleportObjectToY"].GetFloat(), teleporter["teleportObjectToZ"].GetFloat() };
			gameObject->AddComponent<CTeleporterComponent>(*gameObject, teleportersName, teleportTo, position);
		}
	}
}

//void CSceneManager::AddTriggerEvents(CScene& aScene, const std::vector<Binary::SEventData>& someData)
//{
//	for (const auto& triggerEvent : someData)
//	{
//		CGameObject* gameObject = aScene.FindObjectWithID(triggerEvent.instanceID);
//		CBoxColliderComponent* triggerVolume = nullptr;
//		if (gameObject->TryGetComponent<CBoxColliderComponent>(&triggerVolume))
//		{
//			std::string eventData = triggerEvent.gameEvent;
//			int eventFilter = triggerEvent.eventFilter;
//			triggerVolume->RegisterEventTriggerMessage(eventData);
//			triggerVolume->RegisterEventTriggerFilter(eventFilter);
//		}
//	}
//}

CSceneFactory* CSceneFactory::ourInstance = nullptr;
CSceneFactory::CSceneFactory()
{
	myBinReader = new CBinReader();
	ourInstance = this;
}

CSceneFactory::~CSceneFactory()
{
	ourInstance = nullptr;
}

CSceneFactory* CSceneFactory::Get()
{
	return ourInstance;
}

void CSceneFactory::LoadScene(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete)
{
	CScene* loadedScene = CSceneManager::CreateScene(aSceneName);

	CEngine::GetInstance()->AddScene(aState, loadedScene);
	CEngine::GetInstance()->SetActiveScene(aState);

	if (onComplete != nullptr)
		onComplete(aSceneName);
}

void CSceneFactory::LoadSceneAsync(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete)
{
	myOnComplete = onComplete;
	myLastSceneName = aSceneName;
	myLastLoadedState = aState;
	myFuture = std::async(std::launch::async, &CSceneManager::CreateScene, aSceneName);
}

void CSceneFactory::LoadSceneBin(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete)
{
	aState;
	aSceneName;
	onComplete;
	myBinReader->Load(aSceneName);
	std::cout << aSceneName.c_str() << " - Load Scene using Binary Data" << std::endl;
}

void CSceneFactory::LoadSceneAsyncBin(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete)
{
	aSceneName;
	aState;
	onComplete;

}

void CSceneFactory::Update()
{
	if (myFuture._Is_ready())
	{
		CScene* loadedScene = myFuture.get();
		if (loadedScene == nullptr)
		{
			ENGINE_ERROR_MESSAGE("Failed to Load Scene %s", myLastSceneName.c_str());
			return;
		}

		CEngine::GetInstance()->AddScene(myLastLoadedState, loadedScene);
		CEngine::GetInstance()->SetActiveScene(myLastLoadedState);
		myOnComplete(myLastSceneName);
	}
}
