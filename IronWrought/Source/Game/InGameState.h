#pragma once
#include "State.h"
#include "StateStack.h"
#include "InputObserver.h"
#include "Observer.h"
#include "PostMaster.h"

#include <array>

class CGameObject;
class CMouseSelection;
class CCanvas;

class CInGameState : public CState, public IStringObserver, public IObserver
{
public:
	CInGameState(CStateStack& aStateStack, const CStateStack::EState aState = CStateStack::EState::InGame);
	~CInGameState() override;

	void Awake() override;
	void Start() override;
	void Stop() override;
	void Update() override;

	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;

	void OnSceneLoadCompleteMenu(std::string aMsg);
	void OnSceneLoadCompleteInGame(std::string aMsg);

private:
	enum EInGameCanvases
	{
		EInGameCanvases_MainMenu,
		EInGameCanvases_HUD,
		EInGameCanvases_PauseMenu,
		EInGameCanvases_LoadingScreen,
		EInGameCanvases_GameOver,

		EInGameCanvases_Count
	};

	void DEBUGFunctionality();

	void ToggleCanvas(EInGameCanvases anEInGameCanvases);

	void CreateMenuCamera(CScene& aScene);

	void CheckIfExit();
	void UpdateEndCredits();
	void PauseMenu();
	void CheckIfPlay();
	const bool UpdateGameOverTimer();

	void InitCanvases();

private:
	enum class EExitTo
	{
		None,
		MainMenu,
		DelayedWindows,
		Windows,
		AnotherLevel
	};
	EExitTo myExitTo;

	class CEnemyAnimationController* myEnemyAnimationController;

	short myCurrentCameraPositionIndex;
	CGameObject* myMenuCamera;
	std::array<Vector3, 5> myMenuCameraPositions;
	std::array<Vector3, 5> myMenuCameraRotations;
	Vector3 myMenuCameraTargetPosition;
	Quaternion myMenuCameraTargetRotation;
	std::array<CCanvas*, EInGameCanvases_Count> myCanvases;
	EInGameCanvases myCurrentCanvas;
	float myMenuCameraSpeed;
	std::string myCurrentLevel;

	enum class EEndCreditsState
	{
		HasStarted,
		Init,
		FadeInEndCredits,
		ShowCredits,
		FadeOutCredits,
		FadeInMenu,
		None
	};
	EEndCreditsState myEndCreditsState;
	float myEndCreditsTimer;
	const float myEndCreditsFadeInTimer;
	const float myEndCreditsShowForTimer;
	const float myEndCreditsFadeOutTimer;
	const float myMenuFadeInTimer;
	float myGameOverTimer;
	const float myGameOverTimerMax;

	// Used by both quit and start game :U
	float myToFirstCameraPosTimer;
	// Used by both quit and start game :U
	const float myToFirstCameraPosTimerMax;
};