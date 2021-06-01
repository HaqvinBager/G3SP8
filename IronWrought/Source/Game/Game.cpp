#include "stdafx.h"
#include "Game.h"
#include "InGameState.h"
//#include "BinReader.h"
#include <EngineDefines.h>
#include <SceneManager.h>

//#include "MainSingleton.h"
//#include "PostMaster.h"

#ifdef _DEBUG
#pragma comment(lib, "Engine_Debug.lib")
#endif // _DEBUG
#ifdef NDEBUG
#pragma comment(lib, "Engine_Release.lib")
#endif // NDEBUG

CGame::CGame()
{
}

CGame::~CGame()
{
}

void CGame::Init()
{
#ifdef NDEBUG
	InitRealGame();
#else
	InitDev();
#endif
}

bool CGame::Update()
{
	bool stateStackHasUpdated = myStateStack.Update();
	CMainSingleton::PostMaster().FlushEvents();
	return stateStackHasUpdated;
}

void CGame::InitDev()
{
	myStateStack.Awake(
		{
			CStateStack::EState::InGame //This order works, but if InGame is not last in the list we risk getting a crash (pointers to components are still ghosting around).
		},
		CStateStack::EState::InGame);
}

void CGame::InitRealGame()
{
	myStateStack.Awake(
		{
			CStateStack::EState::BootUp,
			CStateStack::EState::InGame
		},
		CStateStack::EState::BootUp);
}
