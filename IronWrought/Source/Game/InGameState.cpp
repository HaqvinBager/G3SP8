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
#include "PlayerControllerComponent.h"

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
#ifdef NDEBUG
#define INGAME_USE_MENU
#else
//#define INGAME_USE_MENU
#endif

#define MENU_SCENE "Level_Cottage_1"

#pragma warning( disable : 26812 )

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myEnemyAnimationController(nullptr)
	, myExitTo(EExitTo::None)
	, myMenuCamera(nullptr)
	, myMenuCameraPositions({ Vector3(10.3f, -0.47f, -37.5f), Vector3(16.63f, -0.24f, -37.5f), Vector3(10.93f, -1.246f, -34.6f), Vector3(17.03f, -0.464f, -33.78f) })
	, myMenuCameraRotations({ Vector3(0.964f, -89.4f, 1.04f) , Vector3(-1.613f, 91.4f, 1.045f) , Vector3(-0.243f, 130.45f, 1.042f) , Vector3(-1.614f, 80.0f, 1.018f) })
	, myCanvases({ nullptr, nullptr, nullptr })
	, myCurrentCanvas(EInGameCanvases_Count)
	, myMenuCameraSpeed(2.0f)
	, myShowCreditsFromEnd(false)
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
	IRONWROUGHT->HideCursor(false);

	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Credits, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::CanvasButtonIndex, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Resume, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetResolution1600x900, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetResolution1920x1080, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetResolution2560x1440, this);
	
	// Removed as of 2021 06 10 / Aki
	/*
		//std::string levelsPath = "Json/Settings/Levels.json";
		//const auto doc = CJsonReader::Get()->LoadDocument(levelsPath);
		//if (doc.HasParseError())
		//{
		//	levelsPath.append(" has a parse error.");
		//	assert(false && levelsPath.c_str());
		//	return;
		//}
		//if (!doc.HasMember("Levels"))
		//{
		//	levelsPath.append(" is missing Levels member!");
		//	assert(false && levelsPath.c_str());
		//	return;
		//}

		//auto levels = doc["Levels"].GetArray();
		//std::vector<std::string> levelNames(levels.Size());
		//for (auto i = 0; i < levelNames.size(); ++i)
		//{
		//	levelNames[i] = levels[i].GetString();
		//}
		//CSceneFactory::Get()->LoadSeveralScenesAsync("All", levelNames, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadComplete(aMsg); });
	*/
	// ! Removed as of 2021 06 10 / Aki

	CMainSingleton::PostMaster().Send({ EMessageType::ClearStaticAudioSources });

	myCurrentLevel = MENU_SCENE;
	CSceneFactory::Get()->LoadSceneAsync(MENU_SCENE, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadCompleteMenu(aMsg); });
	myExitTo = EExitTo::None;

	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_CANVAS, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_TO_MAIN_MENU, this);
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

	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadLevel, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::LevelSelectLoadLevel, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerDied, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EndOfGameEvent, this);


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
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetResolution1600x900, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetResolution1920x1080, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetResolution2560x1440, this);
#endif

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadLevel, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::LevelSelectLoadLevel, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerDied, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EndOfGameEvent, this);


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
		CMainSingleton::PostMaster().SendLate({ EMessageType::UIButtonPress });

		bool isPaused = false;
		if (myCurrentCanvas == EInGameCanvases_HUD)
		{
			ToggleCanvas(EInGameCanvases_PauseMenu);
			isPaused = true;
			CMainSingleton::PostMaster().Send({ EMessageType::PauseMenu, &isPaused });
		}
		else if (myCurrentCanvas == EInGameCanvases_PauseMenu)
		{
			ToggleCanvas(EInGameCanvases_HUD);
			isPaused = false;
			CMainSingleton::PostMaster().Send({ EMessageType::PauseMenu, &isPaused });
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
		ToggleCanvas(EInGameCanvases_MainMenu);
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
		IRONWROUGHT->GetActiveScene().ToggleSections(data->mySceneSection);
		
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
		myCanvases[myCurrentCanvas]->ForceEnabled(false);
		return;
	}
	if (PostMaster::EnableCanvas(aMessage.myMessageType))
	{
		myCanvases[myCurrentCanvas]->ForceEnabled(true);
		return;
	}
}

void CInGameState::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::PlayerDied:
		{
			CMainSingleton::PostMaster().Send({ EMessageType::ClearStaticAudioSources });
			ToggleCanvas(EInGameCanvases_LoadingScreen);
			IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, 0.5f);
			IRONWROUGHT->GetActiveScene().PlayerController()->LockMovementFor(0.5f);
			CMainSingleton::PostMaster().Send({ PostMaster::SMSG_DISABLE_GLOVE, nullptr });
			myMenuCamera = nullptr;
			CSceneFactory::Get()->LoadSceneAsync(myCurrentLevel, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadCompleteInGame(aMsg); });
		}break;

		case EMessageType::LoadLevel:
		{
			CMainSingleton::PostMaster().Send({ EMessageType::ClearStaticAudioSources });
			ToggleCanvas(EInGameCanvases_LoadingScreen);
			IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, 0.5f);
			IRONWROUGHT->GetActiveScene().PlayerController()->LockMovementFor(0.5f);
			CMainSingleton::PostMaster().Send({ PostMaster::SMSG_DISABLE_GLOVE, nullptr });
			myMenuCamera = nullptr;
			myCurrentLevel = *static_cast<std::string*>(aMessage.data);
			CSceneFactory::Get()->LoadSceneAsync(myCurrentLevel, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadCompleteInGame(aMsg); });
		}break;

		case EMessageType::LevelSelectLoadLevel:
		{
			if (!aMessage.data)
				return;

			CMainSingleton::PostMaster().Send({ EMessageType::ClearStaticAudioSources });
			ToggleCanvas(EInGameCanvases_LoadingScreen);
			myMenuCamera = nullptr;
			
			myCurrentLevel = *static_cast<std::string*>(aMessage.data);
			CSceneFactory::Get()->LoadSceneAsync(myCurrentLevel, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadCompleteInGame(aMsg); });
		}break;

		case EMessageType::StartGame:
		{
			//IRONWROUGHT->GetActiveScene().ToggleSections(0);// Disable when single level loading.
			IRONWROUGHT->GetActiveScene().myPlayer->Active(true);
			ToggleCanvas(EInGameCanvases_HUD);
		}break;

		case EMessageType::Resume:
		{
			ToggleCanvas(EInGameCanvases_HUD);
		}break;

		case EMessageType::SetResolution1600x900:
		{
			CEngine::GetInstance()->SetResolution({ 1600.0f, 900.0f });
			myCanvases[EInGameCanvases_MainMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_MainMenu.json"));
			myCanvases[EInGameCanvases_HUD]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_HUD.json"));
			myCanvases[EInGameCanvases_PauseMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_PauseMenu.json"));
			myCanvases[EInGameCanvases_LoadingScreen]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_LoadingScreen.json"));
		} break;
		case EMessageType::SetResolution1920x1080:
		{
			CEngine::GetInstance()->SetResolution({ 1920.0f, 1080.0f });
			myCanvases[EInGameCanvases_MainMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_MainMenu.json"));
			myCanvases[EInGameCanvases_HUD]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_HUD.json"));
			myCanvases[EInGameCanvases_PauseMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_PauseMenu.json"));
			myCanvases[EInGameCanvases_LoadingScreen]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_LoadingScreen.json"));
		} break;
		case EMessageType::SetResolution2560x1440:
		{
			CEngine::GetInstance()->SetResolution({ 2560.0f, 1440.0f });
			myCanvases[EInGameCanvases_MainMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_MainMenu.json"));
			myCanvases[EInGameCanvases_HUD]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_HUD.json"));
			myCanvases[EInGameCanvases_PauseMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_PauseMenu.json"));
			myCanvases[EInGameCanvases_LoadingScreen]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_LoadingScreen.json"));
		} break;

		case EMessageType::MainMenu:
		{
			bool isPaused = false;
			CMainSingleton::PostMaster().Send({ EMessageType::PauseMenu, &isPaused });
			myStateStack.PopTopAndPush(CStateStack::EState::InGame);
		}break;

		case EMessageType::CanvasButtonIndex:
		{
#ifdef INGAME_USE_MENU
			int index = *static_cast<int*>(aMessage.data);
			if (index < 1 || index > myMenuCameraPositions.size() - 1 || myMenuCameraTargetPosition == myMenuCameraPositions[index])
				break;

			myMenuCameraTargetPosition = myMenuCameraPositions[index];
			myMenuCameraTargetRotation = Quaternion::CreateFromYawPitchRoll
			(
				  DirectX::XMConvertToRadians(myMenuCameraRotations[index].y)
				, DirectX::XMConvertToRadians(myMenuCameraRotations[index].x)
				, DirectX::XMConvertToRadians(myMenuCameraRotations[index].z)
			);

			CMainSingleton::PostMaster().SendLate({ EMessageType::UICameraWoosh, nullptr });
#endif
		}break;

		case EMessageType::Quit:
		{
			myExitTo = EExitTo::Windows;
		}break;

		case EMessageType::EndOfGameEvent:
		{
				myShowCreditsFromEnd = true;
				myStateStack.PopTopAndPush(CStateStack::EState::InGame);
		}break;

		default:break;
	}
}

void CInGameState::OnSceneLoadCompleteMenu(std::string /*aMsg*/)
{
	std::cout << __FUNCTION__ << " Menu Load Complete!" << std::endl;
	CScene& scene = IRONWROUGHT->GetActiveScene();
	CreateMenuCamera(scene);

	ToggleCanvas(EInGameCanvases_MainMenu);

	//myCurrentCanvas = EInGameCanvases_MainMenu;
	//scene.SetCanvas(myCanvases[myCurrentCanvas]);
	//scene.UpdateOnlyCanvas(false);
	//IRONWROUGHT->ShowCursor(true);

	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);// Might be redundant.

	myExitTo = EExitTo::None;
}

void CInGameState::OnSceneLoadCompleteInGame(std::string aMsg)
{
	std::cout << __FUNCTION__ << " InGame Load Complete!" << std::endl;
	/*CScene& scene = */IRONWROUGHT->GetActiveScene();
	ToggleCanvas(EInGameCanvases_HUD);
	IRONWROUGHT->HideCursor(false);
	
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);

	int levelIndex = -1;
	if (aMsg == "Level_Cottage_1")
	{
		levelIndex = 0;
	}
	if (aMsg == "Level_Basement1")
	{
		levelIndex = 1;
	}
	if (aMsg == "Level_Basement2")
	{
		levelIndex = 2;
	}
	if (aMsg == "Level_Cottage_2")
	{
		levelIndex = 3;
	}
	if (aMsg == "Level_Basement1_3")
	{
		levelIndex = 4;
	}
	
	if (levelIndex > -1)
	{
		CMainSingleton::PostMaster().Send({ EMessageType::SetAmbience, &levelIndex });
	}

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

	if (Input::GetInstance()->IsKeyPressed('0'))
	{
		CMainSingleton::PostMaster().Send({ EMessageType::PropCollided, IRONWROUGHT->GetActiveScene().Player() });
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
		scene.UpdateOnlyCanvas(false);
		scene.MainCamera(ESceneCamera::MenuCam);
		myMenuCamera->myTransform->Position(myMenuCameraPositions[0]);
		IRONWROUGHT->ShowCursor(false);
		IRONWROUGHT->SetIsMenu(true);
		myCanvases[myCurrentCanvas]->DisableWidgets();

		if (myShowCreditsFromEnd == true)
		{
			myShowCreditsFromEnd = false;
			myCanvases[EInGameCanvases_MainMenu]->ForceEnabled(true);
			myCanvases[EInGameCanvases_MainMenu]->DisableWidgets(0);

			CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
			data.myVignetteStrength = 0.35f;
			CEngine::GetInstance()->SetPostProcessingBufferData(data);
		}
		
		scene.myPlayer->Active(false);
	}
	else if (myCurrentCanvas == EInGameCanvases_PauseMenu)
	{
		scene.UpdateOnlyCanvas(true);
		scene.CanvasIsHUD(false);
		scene.MainCamera(ESceneCamera::PlayerFirstPerson);
		IRONWROUGHT->ShowCursor(false);
		IRONWROUGHT->SetIsMenu(true);
	}
	else if (myCurrentCanvas == EInGameCanvases_HUD)
	{
		scene.UpdateOnlyCanvas(false);
		scene.CanvasIsHUD(true);
		scene.MainCamera(ESceneCamera::PlayerFirstPerson);
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::CanvasButtonIndex, this);
		IRONWROUGHT->HideCursor(false);
		IRONWROUGHT->SetIsMenu(false);
	}
	else if (myCurrentCanvas == EInGameCanvases_LoadingScreen)
	{
		IRONWROUGHT->HideCursor(false);
		scene.SetCanvas(myCanvases[myCurrentCanvas]);
		scene.UpdateOnlyCanvas(true);
		IRONWROUGHT->SetIsMenu(false);
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
	else if (myCurrentCanvas == EInGameCanvases_LoadingScreen)
	{
		CScene& scene = IRONWROUGHT->GetActiveScene();
		scene.SetCanvas(myCanvases[myCurrentCanvas]);
		scene.UpdateOnlyCanvas(true);
		IRONWROUGHT->ShowCursor(false);
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
