#include "stdafx.h"
#include "Scene.h"

#include <algorithm>

#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"

#include "Model.h"
#include "ModelFactory.h"
#include "ModelComponent.h"
#include "InstancedModelComponent.h"

#include "SpriteInstance.h"
#include "AnimatedUIElement.h"
#include "TextInstance.h"

#include "LineInstance.h"
#include "LineFactory.h"

#include "EnvironmentLight.h"
#include "PointLightComponent.h"
#include "PointLight.h"
#include "SpotLightComponent.h"
#include "SpotLight.h"
#include "BoxLight.h"

#include "Engine.h"
#include "PostMaster.h"

#include "Camera.h"
#include "CameraComponent.h"

#include <PlayerControllerComponent.h>
#include <EnemyComponent.h>

#include "CollisionManager.h"

#include "NavmeshLoader.h"
#include "Canvas.h"

#include "Debug.h"
//SETUP START
CScene::CScene(const int aNumberOfSections, const unsigned int aGameObjectCount)
	: myIsReadyToRender(false)
	, myMainCamera(nullptr)
	, myEnvironmentLight(nullptr)
	, myPXScene(nullptr)
	, myPlayer(nullptr)
	, myCanvas(nullptr)
	, myActiveCamera(ESceneCamera::NoCamera)
	, myDeleteCanvas(true)
	, myUpdateOnlyCanvas(false)
	, myCurrentSection(0)
#ifdef _DEBUG
	, myGrid(nullptr)
#endif
{
	SetNumberOfSections(aNumberOfSections);
	for (auto& gameObjects : myGameObjects)
	{
		gameObjects.reserve(aGameObjectCount);
	}
	myPXScene = CEngine::GetInstance()->GetPhysx().CreatePXScene();

	myModelsToOutline.resize(2);
	for (unsigned int i = 0; i < myModelsToOutline.size(); ++i)
	{
		myModelsToOutline[i] = nullptr;
	}

#ifdef _DEBUG
	myShouldRenderLineInstance = false;
	//myGrid = new CLineInstance();
	//myGrid->Init(CLineFactory::GetInstance()->CreateGrid({ 0.1f, 0.5f, 1.0f, 1.0f }));
	//this->AddInstance(myGrid);
#endif
}

CScene::~CScene()
{
	this->ClearGameObjects();
	this->ClearSecondaryEnvironmentLights();
	this->ClearPointLights();
	this->ClearSpotLights();
	this->ClearBoxLights();

	myMainCamera = nullptr;
	delete myEnvironmentLight;
	myEnvironmentLight = nullptr;

	if (myDeleteCanvas)
		delete myCanvas;
	myCanvas = nullptr;

	myVFXTester = nullptr;
	myPlayer = nullptr;
	IRONWROUGHT->SetAudioListener(nullptr);
	CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, nullptr });

	myIDGameObjectMap.clear();
	myComponentMap.clear();

#ifdef _DEBUG
	myGrid = nullptr;
	delete myGrid;
#endif
	if (myPXScene != nullptr)
	{
		myPXScene->release();
		myPXScene = nullptr;
	}

	// Any CScene that is not InGame's scene will not hold a NavMesh

	// Even with this the memory still increases on every load!
}

bool CScene::Init()
{
	return true;
}

bool CScene::InitCanvas(const std::string& aPath)
{
	if (!myDeleteCanvas)
	{
		myCanvas = nullptr;
		myDeleteCanvas = true;
	}

	if (!myCanvas)
		myCanvas = new CCanvas();

	myCanvas->Init(aPath);

	return true;
}
bool CScene::InitCanvas()
{
	if (!myDeleteCanvas)
	{
		myCanvas = nullptr;
		myDeleteCanvas = true;
	}

	if (!myCanvas)
		myCanvas = new CCanvas();

	myCanvas->Init();
	return true;
}
bool CScene::ReInitCanvas(const std::string& aPath, const bool& aDelete)
{
	if (aDelete && myDeleteCanvas)
	{
		delete myCanvas;
		myCanvas = nullptr;
	}

	return InitCanvas(aPath);
}

void CScene::CanvasIsHUD(const bool& aIsHud)
{
	if (myCanvas)
		myCanvas->IsHUDCanvas(aIsHud);
}

void CScene::DisableWidgetsOnCanvas()
{
	if (myCanvas)
		myCanvas->DisableWidgets();
}

void CScene::CanvasToggle(const bool& anIsEnabled, const bool& anIsForceEnable)
{
	if (myCanvas)
	{
		if (anIsForceEnable)
			myCanvas->ForceEnabled(anIsEnabled);
		else
			myCanvas->SetEnabled(anIsEnabled);
	}
}

void CScene::SetCanvas(CCanvas* aCanvas)
{
	if (myDeleteCanvas)
	{
		if (myCanvas)
			delete myCanvas;
		myCanvas = nullptr;
	}
	myDeleteCanvas = false;
	myCanvas = aCanvas;
}

void CScene::UpdateOnlyCanvas(const bool anUpdateOnlyCanvas)
{
	myUpdateOnlyCanvas = anUpdateOnlyCanvas;
}

//No longer needed due to Components Awake being called via EMessageType "AddComponent"
void CScene::Awake()
{
}

void CScene::Start()
{
	IRONWROUGHT->SetAudioListener(myPlayer);

	for (int i = 0; i < myGameObjects.size(); ++i)
	{
		auto enemyComponent = FindFirstObjectWithComponentInSection<CEnemyComponent>(i);
		if (enemyComponent)
		{
			CGameObject* enemy = &enemyComponent->GameObject();
			CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, enemy });
			AddEnemyShortcut(enemy, i);
		}
	}
	//auto enemyComponent = FindFirstObjectWithComponent<CEnemyComponent>();
	//if (enemyComponent)
	//{
	//	CGameObject* enemy = &enemyComponent->GameObject();
	//	CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, enemy });
	//}
}

void CScene::Update()
{
	InitAnyNewComponents();

	if (myUpdateOnlyCanvas)
	{
		myCanvas->Update();
		return;
	}

	// Why not just update (auto& gameObjects : myGameObjects[myCurrentSection)? 
	//		1: Not doing it is faster to get into a working state.
	//		2: Player is in one of the vectors and needs to be updated.
	for (auto& gameObjects : myGameObjects)
	{
		for (auto& gameObject : gameObjects)
			gameObject->Update();

		for (auto& gameObject : gameObjects)
			gameObject->LateUpdate();
	}

	if (myCanvas)
		myCanvas->Update();
}

void CScene::FixedUpdate()
{
	if (myUpdateOnlyCanvas)
		return;

	for (auto& gameObjects : myGameObjects)
	{
		for (auto& gameObject : gameObjects)
			gameObject->FixedUpdate();
	}
}

void CScene::InitAnyNewComponents()
{
	CallAwakeOnNewComponents();
	CallStartOnNewComponents();
}

void CScene::CallStartOnNewComponents()
{
	while (!myStartComponents.empty())
	{
		CComponent* component = myStartComponents.front();
		myStartComponents.pop();
		//component->OnEnable();
		component->Start();
	}
}

void CScene::CallAwakeOnNewComponents()
{
	while (!myAwakeComponents.empty())
	{
		CComponent* component = myAwakeComponents.front();

		const auto& hashCode = typeid(*component).hash_code();
		if (myComponentMap.find(hashCode) == myComponentMap.end())
			myComponentMap[hashCode].push_back(component);
		else
		{
			auto& componentVector = myComponentMap.at(hashCode);
			if (std::find(componentVector.begin(), componentVector.end(), component) == componentVector.end())
				componentVector.push_back(component);
		}

		myAwakeComponents.pop();
		component->Awake();
		component->OnEnable();
		myStartComponents.push(component);
	}
}

void CScene::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::ComponentAdded:
	{
		CComponent* addedComponent = static_cast<CComponent*>(aMessage.data);
		myAwakeComponents.push(addedComponent);
	}
	break;
	}
}

//SETUP END
//SETTERS START
void CScene::MainCamera(const ESceneCamera aCameraType)
{
	ESceneCamera previousCameraType = myActiveCamera;

	if (myCameras.find(aCameraType) != myCameras.end())
	{
		if (myCameras[aCameraType] != nullptr)
		{
			if (aCameraType != previousCameraType)
				myCameras[aCameraType]->GameObject().Active(false);
		}
	}

	myActiveCamera = aCameraType;
	myMainCamera = myCameras[aCameraType];

	if (myCameras[myActiveCamera]->GameObject().Active() == false)
	{
		if (aCameraType != previousCameraType)
			myCameras[myActiveCamera]->GameObject().Active(true);
	}
}

CCameraComponent* CScene::MainCamera()
{
	if (myActiveCamera == ESceneCamera::NoCamera)
		return nullptr;

	return myCameras[myActiveCamera];
}


void CScene::AddCamera(CCameraComponent* aCamera, const ESceneCamera aCameraType)
{
	myCameras[aCameraType] = aCamera;
}

void CScene::Player(CGameObject* aPlayerObject)
{
	myPlayer = aPlayerObject;
}

void CScene::AddEnemyShortcut(CGameObject* anEnemyObject, const int aSection)
{
	if (SectionBoundsCheck(aSection))
	{
		if (myEnemies.find(aSection) == myEnemies.end())
			myEnemies.emplace(aSection, anEnemyObject);
		else
			std::cout << __FUNCTION__ << " Enemy was not added to myEnemies due to one already existing in the requested section." << std::endl;
	}
	else
	{
		std::cout << __FUNCTION__ << " Enemy was not added to myEnemies due to aSection being out of bounds." << std::endl;
	}
}

bool CScene::EnvironmentLight(CEnvironmentLight* anEnvironmentLight)
{
	myEnvironmentLight = anEnvironmentLight;
	return true;
}

void CScene::ShouldRenderLineInstance(const bool aShouldRender)
{
#ifdef  _DEBUG
	myShouldRenderLineInstance = aShouldRender;
#else
	aShouldRender;
#endif //  _DEBUG
}
//SETTERS END
//GETTERS START

CGameObject* CScene::Player()
{
	return myPlayer;
}

CGameObject* CScene::GetEnemyInSection(const int aSection)
{
	if (!SectionBoundsCheck(aSection))
	{
		std::cout << __FUNCTION__ << " aSection is out of bounds!" << std::endl;
		return nullptr;
	}
	if (myEnemies.find(aSection) == myEnemies.end())
	{
		std::cout << __FUNCTION__ << " Enemy has not been added for aSection!" << std::endl;
		return nullptr;
	}
	return myEnemies[aSection];
}

CPlayerControllerComponent* CScene::PlayerController()
{
	if (myPlayer)
		return myPlayer->GetComponent<CPlayerControllerComponent>();
	return nullptr;
}

CEnvironmentLight* CScene::EnvironmentLight()
{
	return myEnvironmentLight;
}


PxScene* CScene::PXScene()
{
	return myPXScene;
}

std::vector<CGameObject*> CScene::ModelsToOutline() const
{
	return myModelsToOutline;
}

std::vector<CPointLight*>& CScene::PointLights()
{
	return myPointLights;
}

CCanvas* CScene::Canvas()
{
	return myCanvas;
}

std::vector<CGameObject*> CScene::AllGameObjects()
{
	std::vector<CGameObject*> go;
	size_t numberOfGameObjects = 0;
	for (auto& gameObjectList : myGameObjects)
	{
		numberOfGameObjects += gameObjectList.size();
	}
	go.reserve(numberOfGameObjects);

	for (auto& gameObjectList : myGameObjects)
	{
		for (unsigned int i = 0; i < gameObjectList.size(); ++i)
		{
			go.push_back(gameObjectList[i]);
		}
	}

	return std::move(go);
}

const std::vector<CPatrolPointComponent*>& CScene::PatrolPoints() const
{
	return myPatrolPoints;
}

const std::vector<CGameObject*>& CScene::ActiveGameObjects() const
{
	return myGameObjects[myCurrentSection];
}
std::vector<CEnvironmentLight*> CScene::CullSecondaryEnvironmentLights(CGameObject* /*aGameObject*/)
{
	return mySecondaryEnvironmentLights;
}
//GETTERS END
//CULLING START
std::vector<CPointLight*> CScene::CullPointLights(std::vector<CGameObject*>& someCulledGameObjects)
{
	std::vector<CPointLight*> culledPointLights;

	CPointLightComponent* comp = nullptr;
	for (auto& go : someCulledGameObjects)
	{
		comp = go->GetComponent<CPointLightComponent>();
		if (comp)
		{
			culledPointLights.push_back(comp->GetPointLight());
		}
	}

	return culledPointLights;
}

std::vector<CSpotLight*> CScene::CullSpotLights(std::vector<CGameObject*>& someCulledGameObjects)
{
	std::vector<CSpotLight*> culledSpotLights;

	CSpotLightComponent* comp = nullptr;
	for (auto& go : someCulledGameObjects)
	{
		comp = go->GetComponent<CSpotLightComponent>();
		if (comp)
		{
			culledSpotLights.push_back(comp->GetSpotLight());
		}
	}

	return culledSpotLights;
}

std::vector<CBoxLight*> CScene::CullBoxLights(CGameObject* /*aGameObject*/)
{
	return myBoxLights;
}

std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> CScene::CullLights(CGameObject* aGameObject)
{
	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> pointLightPair;
	UINT counter = 0;
	for (UINT i = 0; i < myPointLights.size(); ++i)
	{
		float distanceSquared = DirectX::SimpleMath::Vector3::DistanceSquared(myPointLights[i]->GetPosition(), aGameObject->GetComponent<CTransformComponent>()->Transform().Translation());
		float range = myPointLights[i]->GetRange();
		if (distanceSquared < (range * range))
		{
			pointLightPair.second[counter] = myPointLights[i];
			++counter;

			if (counter == LIGHTCOUNT)
			{
				break;
			}
		}
	}

	pointLightPair.first = counter;
	return pointLightPair;
}

const std::vector<CLineInstance*>& CScene::CullLineInstances() const
{
#ifdef _DEBUG
	return myLineInstances;
#else
	return myLineInstances;
#endif
}

const std::vector<SLineTime>& CScene::CullLines() const
{
	return CDebug::GetInstance()->GetLinesTime();
	//return CDebug::GetInstance()->GetLines();
}

LightPair CScene::CullLightInstanced(CInstancedModelComponent* aModelType)
{
	//S�tt s� att Range t�cker objektet l�ngst bort
	//if (myPlayer != nullptr) {
	//	aModelType->GameObject().myTransform->Position(GetPlayer()->myTransform->Position());
	//}

	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> pointLightPair;
	UINT counter = 0;
	for (UINT i = 0; i < myPointLights.size(); ++i)
	{
		float distanceSquared = DirectX::SimpleMath::Vector3::DistanceSquared(myPointLights[i]->GetPosition(), aModelType->GetComponent<CTransformComponent>()->Position());
		float range = myPointLights[i]->GetRange() * 200.0f;
		if (distanceSquared < (range * range))
		{
			pointLightPair.second[counter] = myPointLights[i];
			++counter;

			if (counter == LIGHTCOUNT)
			{
				break;
			}
		}
	}

	pointLightPair.first = counter;
	return pointLightPair;
}

std::vector<CGameObject*> CScene::CullGameObjects(CCameraComponent* aMainCamera)
{
	auto& viewFrustum = aMainCamera->GetViewFrustum();
	DirectX::BoundingSphere currentSphere;
	std::vector<CGameObject*> culledGameObjects;
	// Test to see if this speeds it up a bit:
	size_t gameObjectsTotalSize = 0;
	for (auto& gameObjectsInSection : myGameObjects)
	{
		gameObjectsTotalSize += gameObjectsInSection.size();
	}
	culledGameObjects.reserve(gameObjectsTotalSize);
	// !Test 
// Render only current section.
	//for (auto& gameObject : myGameObjects[myCurrentSection])
	//{
	//	if (gameObject->InstanceID() == PLAYER_CAMERA_ID)
	//	{
	//		culledGameObjects.push_back(gameObject);
	//		continue;
	//	}

	//	if (gameObject->GetComponent<CInstancedModelComponent>())
	//	{
	//		culledGameObjects.push_back(gameObject);
	//		continue;
	//	}

	//	if (!gameObject->Active())// Might cause issues, time will tell. Remove if it does harm. // Aki 2021 05 28
	//		continue;

	//	currentSphere = DirectX::BoundingSphere(gameObject->myTransform->Position(), 24.0f);
	//	if (viewFrustum.Intersects(currentSphere))
	//	{
	//		culledGameObjects.push_back(gameObject);
	//	}
	//}
// ! Render only current section

// Render all sections
	for (auto& gameObjectsInSection : myGameObjects)
	{
		for (auto& gameObject : gameObjectsInSection)
		{
			if (gameObject->InstanceID() == PLAYER_CAMERA_ID)
			{
				culledGameObjects.push_back(gameObject);
				continue;
			}

			if (gameObject->GetComponent<CInstancedModelComponent>())
			{
				culledGameObjects.push_back(gameObject);
				continue;
			}

			if (!gameObject->Active())// Might cause issues, time will tell. Remove if it does harm. // Aki 2021 05 28
				continue;

			currentSphere = DirectX::BoundingSphere(gameObject->myTransform->Position(), 24.0f);
			if (viewFrustum.Intersects(currentSphere))
			{
				culledGameObjects.push_back(gameObject);
			}
		}
	}
	// ! Render all sections
	return culledGameObjects;
}

CGameObject* CScene::FindObjectWithID(const int aGameObjectInstanceID)
{
	if (myIDGameObjectMap.find(aGameObjectInstanceID) == myIDGameObjectMap.end())
		return nullptr;

	return myIDGameObjectMap[aGameObjectInstanceID];
}
CGameObject* CScene::FindObjectWithTag(const std::string aTag)
{
	if (myGameObjectTagMap.find(aTag) == myGameObjectTagMap.end())
		return nullptr;

	return myGameObjectTagMap[aTag][0];
}
std::vector<CGameObject*>* CScene::FindObjectsWithTag(const std::string aTag)
{
	if (myGameObjectTagMap.find(aTag) == myGameObjectTagMap.end())
		return nullptr;

	return &myGameObjectTagMap[aTag];
}
bool CScene::AddInstance(CEnvironmentLight* aSecondaryDirectionalLight)
{
	mySecondaryEnvironmentLights.emplace_back(aSecondaryDirectionalLight);
	return true;
}
//CULLING END
//POPULATE SCENE START
bool CScene::AddInstance(CPointLight* aPointLight)
{
	myPointLights.emplace_back(aPointLight);
	return true;
}

bool CScene::AddInstance(CSpotLight* aSpotLight)
{
	mySpotLights.emplace_back(aSpotLight);
	return true;
}

bool CScene::AddInstance(CBoxLight* aBoxLight)
{
	myBoxLights.emplace_back(aBoxLight);
	return true;
}

bool CScene::AddInstance(CLineInstance* aLineInstance)
{
	myLineInstances.emplace_back(aLineInstance);
	return true;
}

bool CScene::AddInstance(CGameObject* aGameObject, const int aToASection)
{
	//Lägg in dom i en "Next frame i will be initied vector, Then when they are inited we move it into myGameObjects

	if (aToASection < 0)
	{
		myGameObjects[myCurrentSection].push_back(aGameObject);
	}
	else
	{
		if (!SectionBoundsCheck(aToASection))
		{
			std::cout << __FUNCTION__ << " GameObject was not added du to aToASection being out of bounds!" << std::endl;
			return false;
		}
		myGameObjects[aToASection].push_back(aGameObject);
	}
	/*myGameObjects.emplace_back(aGameObject);*/
	myIDGameObjectMap[aGameObject->InstanceID()] = aGameObject;
	myGameObjectTagMap[aGameObject->Tag()].push_back(aGameObject);

	for (auto& component : aGameObject->myComponents) {
		myComponentMap[typeid(*component).hash_code()].push_back(component.get());
	}

	return true;
}

bool CScene::AddInstances(std::vector<CGameObject*>& someGameObjects, const int aToASection)
{
	if (someGameObjects.size() == 0)
		return false;

	for (unsigned int i = 0; i < someGameObjects.size(); ++i)
	{
		AddInstance(someGameObjects[i], aToASection);
		//myGameObjects.emplace_back(someGameObjects[i]);
	}


	//myGameObjects.insert(myGameObjects.end(), someGameObjects.begin(), someGameObjects.end());
	return true;
}

bool CScene::AddInstance(CPatrolPointComponent* aPatrolComponent)
{
	myPatrolPoints.push_back(aPatrolComponent);
	return true;
}

//PhysX
bool CScene::AddPXScene(PxScene* aPXScene)
{
	if (!aPXScene)
	{
		return false;
	}
	myPXScene = aPXScene;
	return true;
}
//POPULATE SCENE END
// 
//REMOVE SPECIFIC INSTANCE START
bool CScene::RemoveInstance(CEnvironmentLight* aSecondaryEnvironmentLight)
{
	for (int i = 0; i < mySecondaryEnvironmentLights.size(); ++i)
	{
		if (aSecondaryEnvironmentLight == mySecondaryEnvironmentLights[i])
		{
			mySecondaryEnvironmentLights.erase(mySecondaryEnvironmentLights.begin() + i);
			return true;
		}
	}
	return false;
}
bool CScene::RemoveInstance(CPointLight* aPointLight)
{
	for (int i = 0; i < myPointLights.size(); ++i)
	{
		if (aPointLight == myPointLights[i])
		{
			//std::swap(myGameObjects[i], myGameObjects[myGameObjects.size() - 1]);
			//myGameObjects.pop_back();
			myPointLights.erase(myPointLights.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::RemoveInstance(CSpotLight* aSpotLight)
{
	for (int i = 0; i < mySpotLights.size(); ++i)
	{
		if (aSpotLight == mySpotLights[i])
		{
			mySpotLights.erase(mySpotLights.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::RemoveInstance(CBoxLight* aBoxLight)
{
	for (int i = 0; i < myBoxLights.size(); ++i)
	{
		if (aBoxLight == myBoxLights[i])
		{
			myBoxLights.erase(myBoxLights.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::RemoveInstance(CGameObject* aGameObject)
{
	for (auto& gameObjects : myGameObjects)
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (aGameObject == gameObjects[i])
			{
				//std::swap(myGameObjects[i], myGameObjects[myGameObjects.size() - 1]);
				//myGameObjects.pop_back();
				gameObjects.erase(gameObjects.begin() + i);
				return true;
			}
		}
	}

	return false;
}
bool CScene::ClearSecondaryEnvironmentLights()
{
	for (auto& p : mySecondaryEnvironmentLights)
	{
		delete p;
		p = nullptr;
	}
	mySecondaryEnvironmentLights.clear();
	return true;
}
//REMOVE SPECIFIC INSTANCE END
// 
//CLEAR SCENE OF INSTANCES START
bool CScene::ClearPointLights()
{
	for (auto& p : myPointLights)
	{
		delete p;
		p = nullptr;
	}
	myPointLights.clear();
	return true;
}

bool CScene::ClearSpotLights()
{
	for (auto& p : mySpotLights)
	{
		delete p;
		p = nullptr;
	}
	mySpotLights.clear();
	return true;
}

bool CScene::ClearBoxLights()
{
	for (auto& p : myBoxLights)
	{
		delete p;
		p = nullptr;
	}
	myBoxLights.clear();
	return true;
}

bool CScene::ClearLineInstances()
{
	for (size_t i = 0; i < myLineInstances.size(); ++i)
	{
		if (myLineInstances[i] != nullptr)
		{
			delete myLineInstances[i];
			myLineInstances[i] = nullptr;
		}
	}
	return false;
}

bool CScene::ClearGameObjects()
{
	for (int o = 0; o < myGameObjects.size(); ++o)
	{
		DisableSection(o);
		for (int i = 0; i < myGameObjects[o].size(); ++i)
		{
			delete myGameObjects[o][i];
			myGameObjects[o][i] = nullptr;
		}
		myGameObjects[o].clear();
	}
	myCurrentSection = -1;
	myGameObjects.clear();

	return true;
}
void CScene::AddSection(const bool aSetToCurrent)
{
	myGameObjects.push_back(std::vector<CGameObject*>());
	if (aSetToCurrent)
		myCurrentSection = static_cast<int>(myGameObjects.size()) - 1;
}
void CScene::SetNumberOfSections(const int aNumberOfSections)
{
	if (myGameObjects.size() > 0)
	{
		std::cout << __FUNCTION__ << " myGameObjects Size is not 0. Will not be resized." << std::endl;
		return;
	}
	myGameObjects.resize(aNumberOfSections);
	myCurrentSection = 0;
}
void CScene::NextSection(const bool aShouldToggle)
{
	myCurrentSection = myCurrentSection + 1 >= myGameObjects.size() ? myCurrentSection : myCurrentSection + 1;
	if (aShouldToggle)
		ToggleSections(myCurrentSection);
}
void CScene::PreviousSection(const bool aShouldToggle)
{
	myCurrentSection = myCurrentSection - 1 < 0 ? myCurrentSection : myCurrentSection - 1;
	if (aShouldToggle)
		ToggleSections(myCurrentSection);
}
void CScene::SetCurrentSection(const int aSection)
{
	if (!SectionBoundsCheck(aSection))
	{
		std::cout << __FUNCTION__ << " Remaining on current section. aSection requested is out of bounds: " << aSection << std::endl;
		return;
	}
	myCurrentSection = aSection;
}
void CScene::ToggleSections(const int aSection)
{
	if (!SectionBoundsCheck(aSection))
	{
		std::cout << __FUNCTION__ << " Won't toggle sections, aSection is out of bounds: " << aSection << std::endl;
		return;
	}
	for (int i = 0; i < myGameObjects.size(); ++i)
	{
		auto& gameObjectsInSection = myGameObjects[i];
		for (auto& gameObject : gameObjectsInSection)
		{
			gameObject->Active(i == aSection);
		}
	}
	myCurrentSection = aSection;
	if (myPlayer)
		myPlayer->Active(true);


	auto enemyComponent = FindFirstObjectWithComponentInSection<CEnemyComponent>(aSection);
	if (enemyComponent)
	{
		CGameObject* enemy = &enemyComponent->GameObject();
		CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, enemy });
		AddEnemyShortcut(enemy, aSection);
	}
	else
	{
		CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, nullptr });
	}

	CMainSingleton::PostMaster().SendLate({ EMessageType::SetAmbience, &myCurrentSection });
}
void CScene::EnableSection(const int aSection)
{
	if (!SectionBoundsCheck(aSection))
	{
		std::cout << __FUNCTION__ << " Can't enable section, aSection is out of bounds: " << aSection << std::endl;
		return;
	}
	auto& gameObjectsInSection = myGameObjects[aSection];
	for (auto& gameObject : gameObjectsInSection)
	{
		gameObject->Active(false);
	}
	myCurrentSection = aSection;
	if (myPlayer)
		myPlayer->Active(true);

	auto enemyComponent = FindFirstObjectWithComponentInSection<CEnemyComponent>(aSection);
	if (enemyComponent)
	{
		CGameObject* enemy = &enemyComponent->GameObject();
		CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, enemy });
		AddEnemyShortcut(enemy, aSection);
	}
	else
	{
		CMainSingleton::PostMaster().Send({ EMessageType::SetDynamicAudioSource, nullptr });
	}
}
void CScene::DisableSection(const int aSection)
{
	if (!SectionBoundsCheck(aSection))
	{
		std::cout << __FUNCTION__ << " Can't disable section, aSection is out of bounds: " << aSection << std::endl;
		return;
	}
	auto& gameObjectsInSection = myGameObjects[aSection];
	for (auto& gameObject : gameObjectsInSection)
	{
		gameObject->Active(false);
	}
	if (myPlayer)
		myPlayer->Active(true);
}
void CScene::EnableCurrent()
{
	EnableSection(myCurrentSection);
}
void CScene::DisableCurrent()
{
	DisableSection(myCurrentSection);
}
void CScene::ToggleToCurrent()
{
	ToggleSections(myCurrentSection);
}
bool CScene::SectionBoundsCheck(const int aSection)
{
	return aSection >= 0 && aSection < myGameObjects.size();
}
//CLEAR SCENE OF INSTANCES END

bool CScene::NearestPlayerComparer::operator()(const CPointLight* a, const CPointLight* b) const
{
	float dist0 = Vector3::DistanceSquared(a->GetPosition(), myPos);
	float dist1 = Vector3::DistanceSquared(b->GetPosition(), myPos);
	return dist0 < dist1;
}