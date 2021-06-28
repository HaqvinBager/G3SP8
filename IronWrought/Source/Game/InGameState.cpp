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
#define END_CREDITS_WIDGET_INDEX 4
#pragma warning( disable : 26812 )

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myEnemyAnimationController(nullptr)
	, myExitTo(EExitTo::None)
	, myMenuCamera(nullptr)
	, myMenuCameraPositions({ Vector3(10.3f, -0.47f, -37.5f), Vector3(16.63f, -0.24f, -37.5f), Vector3(10.93f, -1.246f, -34.6f), Vector3(17.03f, -0.464f, -33.78f), Vector3(10.3f, -0.47f, -37.5f) })
	, myMenuCameraRotations({ Vector3(0.964f, -89.4f, 1.04f) , Vector3(-1.613f, 91.4f, 1.045f) , Vector3(-0.243f, 130.45f, 1.042f) , Vector3(-1.614f, 80.0f, 1.018f), Vector3(0.964f, -89.4f, 1.04f) })
	, myCanvases({ nullptr, nullptr, nullptr })
	, myCurrentCanvas(EInGameCanvases_Count)
	, myMenuCameraSpeed(2.0f)
	, myEndCreditsState(EEndCreditsState::None)
	, myEndCreditsTimer(0.0f)
	, myEndCreditsFadeInTimer(1.5f)
	, myEndCreditsShowForTimer(5.0f)
	, myEndCreditsFadeOutTimer(3.0f)
	, myMenuFadeInTimer(3.0f)
	, myGameOverTimer(0.0f)
	, myGameOverTimerMax(8.0f)
	, myToFirstCameraPosTimer(0.0f)
	, myToFirstCameraPosTimerMax(2.0f)
	, myCurrentCameraPositionIndex(0)
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
	InitCanvases();

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
	CMainSingleton::PostMaster().Subscribe(EMessageType::FinishEndOfGameEvent, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::StartEndOfGameEvent, this);

	myCurrentCameraPositionIndex = 0;
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
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::FinishEndOfGameEvent, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StartEndOfGameEvent, this);

	myMenuCamera = nullptr; // Deleted by Scene on IRONWROUGHT->RemoveScene(..), as it is added as a gameobject.
}

void CInGameState::Update()
{
#ifdef INGAME_USE_MENU

	UpdateEndCredits();
	PauseMenu();
	
	if (myMenuCamera)
	{
		const float t = myMenuCameraSpeed * CTimer::Dt();
		CTransformComponent* menuCamTransform = myMenuCamera->myTransform;
		menuCamTransform->Position(Vector3::Lerp(menuCamTransform->Position(), myMenuCameraTargetPosition, t));
		menuCamTransform->Rotation(Quaternion::Slerp(menuCamTransform->Rotation(), myMenuCameraTargetRotation, t));
	}

	if (UpdateGameOverTimer())
		return;

	CheckIfPlay();
#else
#endif

	CheckIfExit();

	DEBUGFunctionality();
}

void CInGameState::Receive(const SStringMessage& aMessage)
{
	if (PostMaster::CompareStringMessage(aMessage.myMessageType, PostMaster::SMSG_SECTION))
	{
		const PostMaster::SBoxColliderEvenTriggerData* data = static_cast<PostMaster::SBoxColliderEvenTriggerData*>(aMessage.data);
		IRONWROUGHT->GetActiveScene().ToggleSections(data->mySceneSection);
		
		std::cout << __FUNCTION__ << (data->myState == true ? "ToggledSections: " : "DisableSection: ") << data->mySceneSection << std::endl;
		return;
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
#ifdef INGAME_USE_MENU
			ToggleCanvas(EInGameCanvases_GameOver);
#endif // INGAME_USE_MENU
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
			if (myToFirstCameraPosTimer > 0.0f)
				return;

			IRONWROUGHT->GetActiveScene().myPlayer->Active(true);
			ToggleCanvas(EInGameCanvases_HUD);
			myToFirstCameraPosTimer = 0.0f;
			myCurrentCameraPositionIndex = 0;

		}break;

		case EMessageType::Resume:
		{
			ToggleCanvas(EInGameCanvases_HUD);
		}break;

		case EMessageType::SetResolution1600x900:
		{
			CEngine::GetInstance()->SetResolution({ 1600.0f, 900.0f });
			InitCanvases();
		} break;
		case EMessageType::SetResolution1920x1080:
		{
			CEngine::GetInstance()->SetResolution({ 1920.0f, 1080.0f });
			InitCanvases();
		} break;
		case EMessageType::SetResolution2560x1440:
		{
			CEngine::GetInstance()->SetResolution({ 2560.0f, 1440.0f });
			InitCanvases();
		} break;

		case EMessageType::MainMenu:
		{
			CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
			data.myVignetteStrength = 0.35f;
			CEngine::GetInstance()->SetPostProcessingBufferData(data);

			bool isPaused = false;
			CMainSingleton::PostMaster().Send({ EMessageType::PauseMenu, &isPaused });
			myStateStack.PopTopAndPush(CStateStack::EState::InGame);
		}break;

		case EMessageType::CanvasButtonIndex:
		{
#ifdef INGAME_USE_MENU
			int index = *static_cast<int*>(aMessage.data);
			if (index < 0 || index > myMenuCameraPositions.size() - 1 || myMenuCameraTargetPosition == myMenuCameraPositions[index])
				break;

			myMenuCameraTargetPosition = myMenuCameraPositions[index];
			myMenuCameraTargetRotation = Quaternion::CreateFromYawPitchRoll
			(
				  DirectX::XMConvertToRadians(myMenuCameraRotations[index].y)
				, DirectX::XMConvertToRadians(myMenuCameraRotations[index].x)
				, DirectX::XMConvertToRadians(myMenuCameraRotations[index].z)
			);

			CMainSingleton::PostMaster().SendLate({ EMessageType::UICameraWoosh, nullptr });

			if (myCurrentCameraPositionIndex != 0 && index == 0)
			{
				myToFirstCameraPosTimer = myToFirstCameraPosTimerMax;
				IRONWROUGHT->GetActiveScene().CanvasToggle(false, true);
			}
			else 
			{
				myCurrentCameraPositionIndex = (short)index;
				std::cout << "myCurrentCameraPositionIndex " << myCurrentCameraPositionIndex << std::endl;
			}
#endif
		}break;

		case EMessageType::Quit:
		{
			if (myMenuCamera->myTransform->Position() == myMenuCameraPositions[0])
				myExitTo = EExitTo::Windows;
			else
			{
				myToFirstCameraPosTimer = myToFirstCameraPosTimerMax;
				myExitTo = EExitTo::DelayedWindows;
			}

			IRONWROUGHT->GetActiveScene().CanvasToggle(false, true);
			std::cout << "EXIT TO: DelayedWindws " << std::endl;
		}break;

		case EMessageType::FinishEndOfGameEvent:
		{
				myEndCreditsState = EEndCreditsState::Init;
				myStateStack.PopTopAndPush(CStateStack::EState::InGame);
		}break;

		case EMessageType::StartEndOfGameEvent:
		{
			myEndCreditsState = EEndCreditsState::HasStarted;
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

	myExitTo = EExitTo::None;
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);// Might be redundant.

	CCameraControllerComponent* playerCameraController = nullptr;
	if (IRONWROUGHT->GetActiveScene().myPlayer->myTransform->FetchChildren()[0]->GameObject().TryGetComponent(&playerCameraController))
	{
		playerCameraController->SetYaw(-90.0f);
	}

	int levelIndex = 0;
	CMainSingleton::PostMaster().Send({ EMessageType::SetAmbience, &levelIndex });
}

void CInGameState::OnSceneLoadCompleteInGame(std::string aMsg)
{
	std::cout << __FUNCTION__ << " InGame Load Complete!" << std::endl;
	/*CScene& scene = */IRONWROUGHT->GetActiveScene();
	ToggleCanvas(EInGameCanvases_HUD);
	IRONWROUGHT->HideCursor(false);
	
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);

	myExitTo = EExitTo::None;
	myEndCreditsState = EEndCreditsState::None;

	float playerRotation = -90.0f;

	int levelIndex = -1;
	if (aMsg == "Level_Cottage_1")
	{
		levelIndex = 0;
		playerRotation = -90.0f;
	}
	if (aMsg == "Level_Basement1")
	{
		levelIndex = 1;
		playerRotation = -90.0f;
	}
	if (aMsg == "Level_Basement2")
	{
		levelIndex = 2;
		playerRotation = 0.0f;
	}
	if (aMsg == "Level_Cottage_2")
	{
		levelIndex = 3;
		playerRotation = 180.0f;
	}
	if (aMsg == "Level_Basement1_3")
	{
		levelIndex = 4;
		playerRotation = -90.0f;
	}
	
	if (levelIndex > -1)
	{
		CCameraControllerComponent* playerCameraController = nullptr;
		if (IRONWROUGHT->GetActiveScene().myPlayer->myTransform->FetchChildren()[0]->GameObject().TryGetComponent(&playerCameraController))
		{
			playerCameraController->SetYaw(playerRotation);
		}

		CMainSingleton::PostMaster().Send({ EMessageType::SetAmbience, &levelIndex });
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

	switch (myCurrentCanvas)
	{
		case EInGameCanvases_MainMenu:
		{
			scene.CanvasToggle(true, true);
			scene.UpdateOnlyCanvas(false);
			scene.MainCamera(ESceneCamera::MenuCam);
			myMenuCamera->myTransform->Position(myMenuCameraPositions[0]);
			IRONWROUGHT->ShowCursor(false);
			IRONWROUGHT->SetIsMenu(true);
			myCanvases[myCurrentCanvas]->DisableWidgets();

			if (myEndCreditsState == EEndCreditsState::Init)
			{
				myEndCreditsState = EEndCreditsState::FadeInEndCredits;
				myEndCreditsTimer = myEndCreditsFadeInTimer;
				myCanvases[myCurrentCanvas]->EnableWidget(END_CREDITS_WIDGET_INDEX);
				IRONWROUGHT->HideCursor();
				myMenuCamera->Awake();
				myMenuCamera->GetComponent<CCameraComponent>()->Fade(true, myEndCreditsFadeInTimer);
				CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
				data.myVignetteStrength = 0.0f;
				CEngine::GetInstance()->SetPostProcessingBufferData(data);
			}

			scene.myPlayer->Active(false);
		}break;

		case EInGameCanvases_PauseMenu:
		{
			scene.CanvasToggle(true, true);
			scene.UpdateOnlyCanvas(true);
			scene.CanvasIsHUD(false);
			scene.MainCamera(ESceneCamera::PlayerFirstPerson);
			IRONWROUGHT->ShowCursor(false);
			IRONWROUGHT->SetIsMenu(true);
		}break;

		case EInGameCanvases_HUD:
		{
			scene.CanvasToggle(true, true);
			scene.UpdateOnlyCanvas(false);
			scene.CanvasIsHUD(true);
			scene.MainCamera(ESceneCamera::PlayerFirstPerson);
			CMainSingleton::PostMaster().Unsubscribe(EMessageType::CanvasButtonIndex, this);
			IRONWROUGHT->HideCursor(false);
			IRONWROUGHT->SetIsMenu(false);
		}break;

		case EInGameCanvases_LoadingScreen:
		{
			scene.CanvasToggle(true, true);
			IRONWROUGHT->HideCursor(false);
			scene.UpdateOnlyCanvas(true);
			IRONWROUGHT->SetIsMenu(false);
		}break;

		case EInGameCanvases_GameOver:
		{
			scene.CanvasToggle(true, true);
			scene.MainCamera()->Fade(false, myGameOverTimerMax + 2.0f);
			scene.UpdateOnlyCanvas(false);
			IRONWROUGHT->SetIsMenu(false);
			myGameOverTimer = myGameOverTimerMax;
		}break;

		default: break;
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
	myCurrentCameraPositionIndex = 0;

	aScene.AddInstance(myMenuCamera);
	CCameraComponent* camComp = myMenuCamera->GetComponent<CCameraComponent>();
	aScene.AddCamera(camComp, ESceneCamera::MenuCam);
	aScene.MainCamera(ESceneCamera::MenuCam);
}

void CInGameState::CheckIfExit()
{
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

		case EExitTo::DelayedWindows:
		{
			if (myMenuCameraTargetPosition == myMenuCameraPositions[0] && myMenuCamera->myTransform->Position() != myMenuCameraPositions[0])
			{
				myToFirstCameraPosTimer -= CTimer::Dt();
				std::cout << "ExitTo::DelayedWindows: myReturnToFirstCameraPosition: " << myToFirstCameraPosTimer << std::endl;
				if (myToFirstCameraPosTimer <= 0.0f)
				{
					myExitTo = EExitTo::Windows;
				}
			}
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

void CInGameState::UpdateEndCredits()
{
	switch (myEndCreditsState)
	{
		case EEndCreditsState::FadeInEndCredits:
		{
			myEndCreditsTimer -= CTimer::Dt();
			if (myEndCreditsTimer <= 0.0f)
			{
				myEndCreditsTimer = myEndCreditsShowForTimer;
				myEndCreditsState = EEndCreditsState::ShowCredits;
			}
		}break;

		case EEndCreditsState::ShowCredits:
		{
			myEndCreditsTimer -= CTimer::Dt();
			if (myEndCreditsTimer <= 0.0f)
			{
				myEndCreditsState = EEndCreditsState::FadeOutCredits;
				myEndCreditsTimer = myEndCreditsFadeOutTimer;
				myMenuCamera->GetComponent<CCameraComponent>()->Fade(false, myEndCreditsFadeOutTimer, true);
			}
		}break;

		case EEndCreditsState::FadeOutCredits:
		{
			myEndCreditsTimer -= CTimer::Dt();
			if (myEndCreditsTimer <= 0.0f)
			{
				myEndCreditsState = EEndCreditsState::FadeInMenu;
				myEndCreditsTimer = myMenuFadeInTimer;
				myMenuCamera->GetComponent<CCameraComponent>()->Fade(true, myMenuFadeInTimer);
				myCanvases[myCurrentCanvas]->DisableWidget(END_CREDITS_WIDGET_INDEX);
				IRONWROUGHT->ShowCursor();
			}
		}break;

		default:break;
	}
}

void CInGameState::PauseMenu()
{
	if (myCurrentCanvas != EInGameCanvases_HUD)
		if (myCurrentCanvas != EInGameCanvases_PauseMenu)
			return;

	if (myEndCreditsState != EEndCreditsState::None)
		return;

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
}

void CInGameState::CheckIfPlay()
{
	if (myExitTo != EExitTo::None)
		return;
	if (myCurrentCameraPositionIndex == myMenuCameraPositions.size() - 1)// Quit should be the last index
		return;

	if (myCurrentCanvas == EInGameCanvases_MainMenu)
	{
		if (myCurrentCameraPositionIndex != 0 && myMenuCameraTargetPosition == myMenuCameraPositions[0] && myMenuCamera->myTransform->Position() != myMenuCameraPositions[0])
		{
			myToFirstCameraPosTimer -= CTimer::Dt();
			std::cout << "myReturnToFirstCameraPosition: " << myToFirstCameraPosTimer << std::endl;
			if (myToFirstCameraPosTimer <= 0.0f)
			{
				IRONWROUGHT->GetActiveScene().myPlayer->Active(true);
				ToggleCanvas(EInGameCanvases_HUD);
				myCurrentCameraPositionIndex = 0;
			}
		}
	}
}

const bool CInGameState::UpdateGameOverTimer()
{
	if (myCurrentCanvas == EInGameCanvases_GameOver)
	{
		myGameOverTimer -= CTimer::Dt();
		if (myGameOverTimer <= 0.0f)
		{
			CMainSingleton::PostMaster().Send({ EMessageType::ClearStaticAudioSources });
			ToggleCanvas(EInGameCanvases_LoadingScreen);
			IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, 0.5f);
			IRONWROUGHT->GetActiveScene().PlayerController()->LockMovementFor(0.5f);
			CMainSingleton::PostMaster().Send({ PostMaster::SMSG_DISABLE_GLOVE, nullptr });
			myMenuCamera = nullptr;
			CSceneFactory::Get()->LoadSceneAsync(myCurrentLevel, myState, [this](std::string aMsg) { CInGameState::OnSceneLoadCompleteInGame(aMsg); });
			return true;
		}
	}
	return false;
}

void CInGameState::InitCanvases()
{
	myCanvases[EInGameCanvases_MainMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_MainMenu.json"));
	myCanvases[EInGameCanvases_HUD]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_HUD.json"));
	myCanvases[EInGameCanvases_PauseMenu]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_PauseMenu.json"));
	myCanvases[EInGameCanvases_LoadingScreen]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_LoadingScreen.json"));
	myCanvases[EInGameCanvases_GameOver]->Init(ASSETPATH("Assets/IronWrought/UI/JSON/UI_GameOver.json"));
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
