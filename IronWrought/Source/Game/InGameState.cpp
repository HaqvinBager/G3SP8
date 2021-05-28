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
//#define INGAME_USE_MENU

#pragma warning( disable : 26812 )

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myEnemyAnimationController(nullptr)
	, myExitTo(EExitTo::None)
	, myMenuCamera(nullptr)
	, myMenuCameraPositions({ Vector3(-15.0f, 7.0f, -75.55f), Vector3(-16.0f, 5.7f, -95.55f), Vector3(-16.0f, 7.0f, -99.0f), Vector3(-12.0f, 7.7f, -94.55f)})
	, myCanvases({ nullptr, nullptr, nullptr })
	, myCurrentCanvas(EInGameCanvases_Count)
{
}

CInGameState::~CInGameState() 
{
	if(myEnemyAnimationController)
		delete myEnemyAnimationController;

	for (size_t i = 0; i < myCanvases.size(); ++i)
	{
		if(myCanvases[i])
			delete myCanvases[i];
	}
}

void CInGameState::Awake()
{
	CJsonReader::Get()->InitFromGenerated();
	myEnemyAnimationController = new CEnemyAnimationController();

#ifdef INGAME_USE_MENU
	for (size_t i = 0; i < myCanvases.size(); ++i)
	{
		myCanvases[i] = new CCanvas();
	}
	myCanvases[EInGameCanvases_MainMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_MainMenu.json"));
	myCanvases[EInGameCanvases_HUD]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_HUD.json"));
	myCanvases[EInGameCanvases_PauseMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_PauseMenu.json"));
#else
	CScene* scene = CSceneManager::CreateEmpty();
	CEngine::GetInstance()->AddScene(myState, scene);
#ifndef NDEBUG
	TEMP_VFX(scene);
#endif
#endif
}

void CInGameState::Start()
{
#ifdef INGAME_USE_MENU
	//CScene* scene = CSceneManager::CreateScene("Level_Cottage");
	std::vector<std::string> levels(3);
	levels[0] = "Level_Cottage";
	levels[1] = "Level_Basement1";
	levels[2] = "Level_Basement2";
	CScene* scene = CSceneManager::CreateSceneFromSeveral(levels);
	Vector3 playerPos = scene->Player()->myTransform->Position();
	Vector3 firstPos = playerPos + scene->Player()->myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraComponent>()->GameObject().myTransform->Position();
	//Quaternion playerRot = scene->Player()->myTransform->Rotation();
	Quaternion playerRot = scene->Player()->myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraComponent>()->GameObject().myTransform->Rotation();
	myMenuCamera = new CGameObject(0);
	CCameraComponent* camComp = myMenuCamera->AddComponent<CCameraComponent>(*myMenuCamera);//Default Fov is 70.0f
	myMenuCamera->AddComponent<CCameraControllerComponent>(*myMenuCamera, 1.0f, CCameraControllerComponent::ECameraMode::MenuCam); //Default speed is 2.0f
	myMenuCamera->myTransform->Position(firstPos);
	myMenuCamera->myTransform->Rotation(playerRot);
	myMenuCameraPositions[0] = firstPos;
	scene->AddInstance(myMenuCamera);
	scene->AddCamera(camComp, ESceneCamera::MenuCam);
	scene->MainCamera(ESceneCamera::MenuCam);

	CEngine::GetInstance()->AddScene(myState, scene);
	scene->SetCanvas(myCanvases[EInGameCanvases_MainMenu]);
	myCurrentCanvas = EInGameCanvases_MainMenu;
	scene->UpdateOnlyCanvas(false);

	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Credits, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::CanvasButtonIndex, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Resume, this);

#endif

	IRONWROUGHT->ShowCursor(true);
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);

	myExitTo = EExitTo::None;

	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_TO_MAIN_MENU, this);
}

void CInGameState::Stop()
{
	IRONWROUGHT->RemoveScene(myState);
	CMainSingleton::CollisionManager().ClearColliders();
	myEnemyAnimationController->Deactivate();

	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_DISABLE_CANVAS, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_ENABLE_CANVAS, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_TO_MAIN_MENU, this);
#ifdef INGAME_USE_MENU
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Credits, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::CanvasButtonIndex, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Resume, this);
#endif
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
		else if(myCurrentCanvas == EInGameCanvases_PauseMenu)
		{
			ToggleCanvas(EInGameCanvases_HUD);
			CMainSingleton::PostMaster().SendLate({ EMessageType::PauseMenu, nullptr });
		}
	}
#endif

	DEBUGFunctionality();

	switch (myExitTo)
	{
		case EExitTo::AnotherLevel:
		{
			myExitTo = EExitTo::None;
			myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
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
	if (PostMaster::LevelCheck(aMessage.myMessageType))
	{
		myExitTo = EExitTo::AnotherLevel;
	}
	if (PostMaster::CompareStringMessage(aMessage.myMessageType, PostMaster::SMSG_TO_MAIN_MENU))
	{
		myExitTo = EExitTo::MainMenu;
	}

	if (PostMaster::DisableCanvas(aMessage.myMessageType))
	{
		IRONWROUGHT->GetActiveScene().CanvasToggle(false);
	}
	if (PostMaster::EnableCanvas(aMessage.myMessageType))
	{
		IRONWROUGHT->GetActiveScene().CanvasToggle(true);
	}
}

void CInGameState::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::StartGame:
		{
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

			myMenuCamera->myTransform->Position(myMenuCameraPositions[index]);
#endif
		}break;

		case EMessageType::Quit:
		{
			myExitTo = EExitTo::Windows;
		}break;

		default:break;
	}
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
#endif
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
