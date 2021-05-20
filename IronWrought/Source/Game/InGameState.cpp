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

#ifdef VERTICAL_SLICE
	bool gHasPlayedAudio = false;
#endif

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myEnemyAnimationController(nullptr)
	, myExitTo(EExitTo::None)
{
}

CInGameState::~CInGameState() 
{
	delete myEnemyAnimationController;
}

//CGameObject* enemy = new CGameObject(919);
void CInGameState::Awake()
{
	CJsonReader::Get()->InitFromGenerated();
	myEnemyAnimationController = new CEnemyAnimationController();
	CScene* scene = CSceneManager::CreateEmpty();

#ifndef NDEBUG
	TEMP_VFX(scene);
#endif
	CEngine::GetInstance()->AddScene(myState, scene);
}


void CInGameState::Start()
{
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->SetBrokenScreen(false);
	IRONWROUGHT->GetActiveScene().CanvasIsHUD();
	IRONWROUGHT->HideCursor();

	myExitTo = EExitTo::None;

#ifdef VERTICAL_SLICE
	if (gHasPlayedAudio == false)
	{
		gHasPlayedAudio = true;
		CMainSingleton::PostMaster().SendLate({ EMessageType::GameStarted, nullptr });
	}
#endif

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
}

void CInGameState::Update()
{
	if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE))
	{
		myStateStack.PushState(CStateStack::EState::PauseMenu);
	}

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
			myStateStack.PopUntil(CStateStack::EState::MainMenu);
#ifdef VERTICAL_SLICE
			gHasPlayedAudio = false;
#endif
		}break;

		case EExitTo::None:
		break;

		default:break;
	}
}

void CInGameState::ReceiveEvent(const EInputEvent aEvent)
{
	if (this == myStateStack.GetTop())
	{
		switch (aEvent)
		{
			case IInputObserver::EInputEvent::PauseGame:
				//myStateStack.PushState(CStateStack::EState::PauseMenu);
				break;
			default:
				break;
		}
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

void CInGameState::Receive(const SMessage& /*aMessage*/)
{
	//switch (aMessage.myMessageType)
	//{
	//	default:break;
	//}
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
