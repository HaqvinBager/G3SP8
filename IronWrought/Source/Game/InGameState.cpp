#include "stdafx.h"
#include "InGameState.h"
#include <GameObject.h>
#include <TransformComponent.h>
#include <ModelComponent.h>
#include <DecalComponent.h>
#include <PointLightComponent.h>
#include <Scene.h>
#include <Engine.h>
#include <PostMaster.h>
#include <MainSingleton.h>
#include <CollisionManager.h>
#include <PointLight.h>
#include <CameraComponent.h>
#include "CameraControllerComponent.h"
#include <Canvas.h>

#include <JsonReader.h>
#include <SceneManager.h>

#include "EnemyAnimationController.h"

#ifndef NDEBUG
	#include <VFXSystemComponent.h>
	#include <VFXMeshFactory.h>
	#include <ParticleEmitterFactory.h>

	#include <TextFactory.h>
	#include <TextInstance.h>
	#include "EnemyComponent.h"

	#include "AnimationComponent.h"
	#include "animationLoader.h"
	#include "ModelComponent.h"

	#include "EngineDefines.h"

	void TEMP_VFX(CScene* aScene);
#endif

// This is a temporary define. Its current use is so that we don't have to deal with the WIP menu. // Aki 2021 05 25
#ifdef NDEBUG
#define INGAME_USE_MENU
#else
//#define INGAME_USE_MENU
#endif // NDEBUG


#pragma warning( disable : 26812 )

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myEnemyAnimationController(nullptr)
	, myExitTo(EExitTo::None)
	, myMenuCamera(nullptr)
	, myMenuCameraPositions({ Vector3(10.28f, -0.55f, -36.5f), Vector3(15.59f, -0.64f, -37.5f), Vector3(17.36f, -1.04f, -27.89f), Vector3(17.03f, -0.464f, -33.78f) })
	, myMenuCameraRotations({ Vector3(-13.4f, 16.5f, 1.05f) , Vector3(0.0f, 85.221f, 0.0f) , Vector3(-20.59f, -30.739f, 0.119f) , Vector3(-1.614f, 80.0f, 1.018f) })
	, myCanvases({ nullptr, nullptr, nullptr })
	, myCurrentCanvas(EInGameCanvases_Count)
	, myMenuCameraSpeed(2.0f)
{
}

CInGameState::~CInGameState()
{
	if (myEnemyAnimationController)
		delete myEnemyAnimationController;

	for (size_t i = 0; i < myCanvases.size(); ++i)
	{
		if (myCanvases[i])
			delete myCanvases[i];
	}
}

void CInGameState::Awake()
{
	CJsonReader::Get()->InitFromGenerated();
	myEnemyAnimationController = new CEnemyAnimationController();

	for (size_t i = 0; i < myCanvases.size(); ++i)
	{
		myCanvases[i] = new CCanvas();
	}
	myCanvases[EInGameCanvases_MainMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_MainMenu.json"));
	myCanvases[EInGameCanvases_HUD]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_HUD.json"));
	myCanvases[EInGameCanvases_PauseMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_PauseMenu.json"));
	myCanvases[EInGameCanvases_LoadingScreen]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_LoadingScreen.json"));

#ifdef INGAME_USE_MENU

#else
	CScene* scene = CSceneManager::CreateEmpty();
	CEngine::GetInstance()->AddScene(myState, scene);
#ifndef NDEBUG
	TEMP_VFX(scene);
	myCanvases[EInGameCanvases_HUD]->SetEnabled(false);
#endif
#endif
}

void CInGameState::Start()
{
#ifdef INGAME_USE_MENU
	CScene* scene = CSceneManager::CreateEmpty();
	myCurrentCanvas = EInGameCanvases_LoadingScreen;
	scene->SetCanvas(myCanvases[myCurrentCanvas]);
	scene->UpdateOnlyCanvas(true);
	CEngine::GetInstance()->AddScene(myState, scene);
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->ShowCursor(false);

	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Credits, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::CanvasButtonIndex, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Resume, this);

	std::string levelsPath = "Json/Settings/Levels.json";
	const auto doc = CJsonReader::Get()->LoadDocument(levelsPath);
	if (doc.HasParseError())
	{
		levelsPath.append(" has a parse error.");
		assert(false && levelsPath.c_str());
		return;
	}
	if (!doc.HasMember("Levels"))
	{
		levelsPath.append(" is missing Levels member!");
		assert(false && levelsPath.c_str());
		return;
	}

	auto levels = doc["Levels"].GetArray();
	std::vector<std::string> levelNames(levels.Size());
	for (auto i = 0; i < levelNames.size(); ++i)
	{
		levelNames[i] = levels[i].GetString();
	}
	CSceneFactory::Get()->LoadSeveralScenesAsync("All", levelNames, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadComplete(aMsg); });

	myExitTo = EExitTo::None;

	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_TO_MAIN_MENU, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_SECTION, this);
#else
	IRONWROUGHT->ShowCursor(true);
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);

	myExitTo = EExitTo::None;

	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_TO_MAIN_MENU, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_SECTION, this);
#endif
}

void CInGameState::Stop()
{
	IRONWROUGHT->RemoveScene(myState);
	CMainSingleton::CollisionManager().ClearColliders();
	myEnemyAnimationController->Deactivate();

	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_DISABLE_CANVAS, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_ENABLE_CANVAS, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_TO_MAIN_MENU, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_SECTION, this);
#ifdef INGAME_USE_MENU
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Credits, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::CanvasButtonIndex, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Resume, this);
#endif

	myMenuCamera = nullptr; // Has been deleted by Scene when IRONWROUGHT->RemoveScene(..) was called, as it is added as a gameobject.
}

void CInGameState::Update()
{
	if (Input::GetInstance()->IsKeyPressed('K'))
	{
		CMainSingleton::PostMaster().Send({ "KeyDown_K", nullptr });
	}

#ifdef INGAME_USE_MENU
	if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE))
	{
		if (myCurrentCanvas == EInGameCanvases_HUD)
		{
			ToggleCanvas(EInGameCanvases_PauseMenu);
		}
		else if (myCurrentCanvas == EInGameCanvases_PauseMenu)
		{
			ToggleCanvas(EInGameCanvases_HUD);
			CMainSingleton::PostMaster().SendLate({ EMessageType::PauseMenu, nullptr });
		}
	}
	if (myMenuCamera)
	{
		myMenuCamera->myTransform->Position(Vector3::Lerp(myMenuCamera->myTransform->Position(), myMenuCameraTargetPosition, myMenuCameraSpeed * CTimer::Dt()));
		myMenuCamera->myTransform->Rotation(Quaternion::Slerp(myMenuCamera->myTransform->Rotation(), myMenuCameraTargetRotation, myMenuCameraSpeed * CTimer::Dt()));
	}
#else
#endif

	DEBUGFunctionality();

	switch (myExitTo)
	{
	case EExitTo::AnotherLevel:
	{
		myExitTo = EExitTo::None;
		//myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
	}break;

	case EExitTo::MainMenu:
	{
		myExitTo = EExitTo::None;
	}break;

	case EExitTo::Windows:
	{
		myStateStack.PopState();
	}break;

	case EExitTo::None:
		break;

	default:break;
	}
}

void CInGameState::Receive(const SStringMessage& aMessage)
{
	if (PostMaster::CompareStringMessage(aMessage.myMessageType, PostMaster::SMSG_SECTION))
	{
		const PostMaster::SBoxColliderEvenTriggerData* data = static_cast<PostMaster::SBoxColliderEvenTriggerData*>(aMessage.data);
		if (data->myState)
			IRONWROUGHT->GetActiveScene().ToggleSections(data->mySceneSection);
		else
			IRONWROUGHT->GetActiveScene().DisableSection(data->mySceneSection);
		
		std::cout << __FUNCTION__ << (data->myState == true ? "ToggledSections: " : "DisableSection: ") << data->mySceneSection << std::endl;
	}

	if (PostMaster::LevelCheck(aMessage.myMessageType))
	{
		myExitTo = EExitTo::AnotherLevel;
		return;
	}
	if (PostMaster::CompareStringMessage(aMessage.myMessageType, PostMaster::SMSG_TO_MAIN_MENU))
	{
		myExitTo = EExitTo::MainMenu;
		return;
	}

	if (PostMaster::DisableCanvas(aMessage.myMessageType))
	{
		IRONWROUGHT->GetActiveScene().CanvasToggle(false);
		return;
	}
	if (PostMaster::EnableCanvas(aMessage.myMessageType))
	{
		IRONWROUGHT->GetActiveScene().CanvasToggle(true);
		return;
	}
}

void CInGameState::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::StartGame:
	{
		IRONWROUGHT->GetActiveScene().ToggleSections(0);// Disable when single level loading.
		ToggleCanvas(EInGameCanvases_HUD);
	}break;

	case EMessageType::Resume:
	{
		ToggleCanvas(EInGameCanvases_HUD);
	}break;

	case EMessageType::SetResolution1280x720:
	{

	}break;

	case EMessageType::SetResolution1600x900:
	{

	}break;

	case EMessageType::SetResolution1920x1080:
	{

	}break;

	case EMessageType::MainMenu:
	{
		// Don't forget a loading screen.
		myStateStack.PopTopAndPush(CStateStack::EState::InGame);
	}break;

	case EMessageType::CanvasButtonIndex:
	{
#ifdef INGAME_USE_MENU
		int index = *static_cast<int*>(aMessage.data);
		if (index < 0 || index > myMenuCameraPositions.size() - 1)
			break;

		myMenuCameraTargetPosition = myMenuCameraPositions[index];
		myMenuCameraTargetRotation = Quaternion::CreateFromYawPitchRoll
		(
			  DirectX::XMConvertToRadians(myMenuCameraRotations[index].y)
			, DirectX::XMConvertToRadians(myMenuCameraRotations[index].x)
			, DirectX::XMConvertToRadians(myMenuCameraRotations[index].z)
		);
#endif
	}break;

	case EMessageType::Quit:
	{
		myExitTo = EExitTo::Windows;
	}break;

	default:break;
	}
}

void CInGameState::OnSceneLoadComplete(std::string /*aMsg*/)
{
	std::cout << __FUNCTION__ << " All Scenes Loaded Succesfully." << std::endl;
	CScene& scene = IRONWROUGHT->GetActiveScene();
	CreateMenuCamera(scene);

	myCurrentCanvas = EInGameCanvases_MainMenu;
	scene.SetCanvas(myCanvases[myCurrentCanvas]);
	scene.UpdateOnlyCanvas(false);
	scene.ToggleSections(0);

	IRONWROUGHT->ShowCursor(true);
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);// Might be redundant.

	myExitTo = EExitTo::None;
}

void CInGameState::DEBUGFunctionality()
{
#ifdef _DEBUG
	if (Input::GetInstance()->IsKeyPressed('X'))
	{
		SStringMessage msg = {};
		msg.data = nullptr;
		msg.myMessageType = PostMaster::SMSG_DISABLE_GLOVE;
		CMainSingleton::PostMaster().Send(msg);

		SStringMessage msg2 = {};
		msg2.data = nullptr;
		msg2.myMessageType = PostMaster::SMSG_DISABLE_CANVAS;
		CMainSingleton::PostMaster().Send(msg2);
	}
	if (Input::GetInstance()->IsKeyPressed('Z'))
	{
		SStringMessage msg = {};
		msg.data = nullptr;
		msg.myMessageType = PostMaster::SMSG_ENABLE_GLOVE;
		CMainSingleton::PostMaster().Send(msg);

		SStringMessage msg2 = {};
		msg2.data = nullptr;
		msg2.myMessageType = PostMaster::SMSG_ENABLE_CANVAS;
		CMainSingleton::PostMaster().Send(msg2);
	}

#ifndef INGAME_USE_MENU
	// TEMP
	ToggleCanvas(EInGameCanvases_HUD);
	// ! TEMP
#endif

	//if (Input::GetInstance()->IsKeyPressed('1'))
	//{
	//	IRONWROUGHT->GetActiveScene().ToggleSections(0);
	//	std::cout << __FUNCTION__ << " toggle to 0" << std::endl;
	//}
	//if (Input::GetInstance()->IsKeyPressed('2'))
	//{
	//	IRONWROUGHT->GetActiveScene().ToggleSections(1);
	//	std::cout << __FUNCTION__ << " toggle to 1" << std::endl;
	//}
#endif
}

void CInGameState::ToggleCanvas(EInGameCanvases anEInGameCanvases)
{
	myCurrentCanvas = anEInGameCanvases;
#ifdef INGAME_USE_MENU
	CScene& scene = IRONWROUGHT->GetActiveScene();
	scene.SetCanvas(myCanvases[myCurrentCanvas]);

	if (myCurrentCanvas == EInGameCanvases_MainMenu)
	{
		scene.UpdateOnlyCanvas(true);
		scene.MainCamera(ESceneCamera::MenuCam);
		myMenuCamera->myTransform->Position(myMenuCameraPositions[0]);
		IRONWROUGHT->ShowCursor();
	}
	else if (myCurrentCanvas == EInGameCanvases_PauseMenu)
	{
		scene.UpdateOnlyCanvas(true);
		scene.CanvasIsHUD(false);
		scene.MainCamera(ESceneCamera::PlayerFirstPerson);
		CMainSingleton::PostMaster().SendLate({ EMessageType::PauseMenu, nullptr });
		IRONWROUGHT->ShowCursor();
	}
	else if (myCurrentCanvas == EInGameCanvases_HUD)
	{
		scene.UpdateOnlyCanvas(false);
		scene.CanvasIsHUD(true);
		scene.MainCamera(ESceneCamera::PlayerFirstPerson);
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::CanvasButtonIndex, this);
		IRONWROUGHT->HideCursor();
	}
#else
	if (myCurrentCanvas == EInGameCanvases_HUD)
	{
		CScene& scene = IRONWROUGHT->GetActiveScene();
		scene.SetCanvas(myCanvases[myCurrentCanvas]);
		scene.UpdateOnlyCanvas(false);
		scene.CanvasIsHUD(true);
		myCanvases[myCurrentCanvas]->SetEnabled(true);
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::CanvasButtonIndex, this);
	}
#endif
}

void CInGameState::CreateMenuCamera(CScene& aScene)
{
	if (!myMenuCamera)
	{
		myMenuCamera = new CGameObject(0);
		myMenuCamera->AddComponent<CCameraControllerComponent>(*myMenuCamera, 1.0f, CCameraControllerComponent::ECameraMode::MenuCam);
		myMenuCamera->AddComponent<CCameraComponent>(*myMenuCamera, CCameraComponent::SP8_FOV);
	}

	Vector3 playerPos = aScene.Player()->myTransform->Position();
	//myMenuCameraPositions[0] = playerPos +  aScene.Player()->myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraComponent>()->GameObject().myTransform->Position();
	Quaternion playerRot = aScene.Player()->myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraComponent>()->GameObject().myTransform->Rotation();

	myMenuCameraTargetPosition = myMenuCameraPositions[0];
	//myMenuCameraTargetRotation = playerRot;
	myMenuCameraTargetRotation = Quaternion::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(myMenuCameraRotations[0].y)
		, DirectX::XMConvertToRadians(myMenuCameraRotations[0].x)
		, DirectX::XMConvertToRadians(myMenuCameraRotations[0].z)
	);

	myMenuCamera->myTransform->Position(myMenuCameraPositions[0]);
	myMenuCamera->myTransform->Rotation(myMenuCameraTargetRotation);

	aScene.AddInstance(myMenuCamera);
	CCameraComponent* camComp = myMenuCamera->GetComponent<CCameraComponent>();
	aScene.AddCamera(camComp, ESceneCamera::MenuCam);
	aScene.MainCamera(ESceneCamera::MenuCam);
}

#ifndef NDEBUG
void TEMP_VFX(CScene* /*aScene*/)
{
	//static int id = 500;
	//CGameObject* abilityObject = new CGameObject(id++);
	//abilityObject->AddComponent<CVFXSystemComponent>(*abilityObject, ASSETPATH("Assets/IronWrought/VFX/JSON/VFXSystem_ToLoad.json"));
	//
	//aScene->AddInstance(abilityObject);
	//aScene->SetVFXTester(abilityObject);
}
#endif
