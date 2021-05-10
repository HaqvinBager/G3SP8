#include "stdafx.h"
#include "StateStack.h"
#include "State.h"
#include "Engine.h"

#include <BootUpState.h>
#include <MainMenuState.h>
#include <InGameState.h>
#include <PauseMenuState.h>
#include <LoadLevelState.h>

CStateStack::~CStateStack()
{
	auto it = myStateMap.begin();
	while (it != myStateMap.end())
	{
		delete it->second;
		it->second = nullptr;
		++it;
	}

}

bool CStateStack::Awake(std::initializer_list<CStateStack::EState> someStates, const EState aStartState)
{
	for (const CStateStack::EState state : someStates)
	{
		switch (state)
		{
		case CStateStack::EState::BootUp:
			myStateMap[state] = new CBootUpState(*this);
			myStateMap[state]->Awake();
			break;
		case CStateStack::EState::MainMenu:
			myStateMap[state] = new CMainMenuState(*this);
			myStateMap[state]->Awake();
			break;
		case CStateStack::EState::InGame:
			myStateMap[state] = new CInGameState(*this);
			myStateMap[state]->Awake();
			break;
		case CStateStack::EState::PauseMenu:
			myStateMap[state] = new CPauseMenuState(*this);
			myStateMap[state]->Awake();
			break;
		case CStateStack::EState::LoadLevel:
			myStateMap[state] = new CLoadLevelState(*this);
			myStateMap[state]->Awake();
			break;
		default:
			assert(0 && "State is not implemented yet.");
			break;
		}
	}

	return PushState(aStartState);
}

bool CStateStack::PushState(const EState aState)
{
	if (myStateMap.find(aState) == myStateMap.end())
		return false;

	myStateStack.push(myStateMap[aState]);
	myStateStack.top()->Start();
	return true;
}

bool CStateStack::PopState()
{
	ENGINE_ERROR_BOOL_MESSAGE(!myStateStack.empty(), "Trying to pop an empty stack");
	myStateStack.top()->Stop();
	/*CEngine::GetInstance()->RemoveScene(myStateStack.top()->myState);*/
	myStateStack.pop();
	return true;
}

bool CStateStack::PopUntil(const EState aState, const bool& anAllowError)
{
	if (myStateStack.empty() && !anAllowError)
		return false;

	ENGINE_ERROR_BOOL_MESSAGE(!myStateStack.empty(), "Trying to pop an empty stack");

	if (myStateStack.top()->GetState() != aState) {
		//ENGINE_ERROR_BOOL_MESSAGE(myStateStack.size() != 1, "Trying to pop until a state that is not in the stack");
		myStateStack.top()->Stop();
		myStateStack.pop();
		return PopUntil(aState, anAllowError);
	}
	myStateStack.top()->Start();
	return true;
}

bool CStateStack::PopTopAndPush(const EState aState)
{
	if (!PopState())
		return false;

	return PushState(aState);
}

void CStateStack::Awake()
{
	myStateStack.top()->Awake();
}

bool CStateStack::Update()
{
	if(!myStateStack.empty())
		myStateStack.top()->Update();

	return myStateStack.size() > 0;
}