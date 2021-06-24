#include "stdafx.h"
#include "BootUpState.h"

#include <SceneManager.h>
#include <Scene.h>

#include <SpriteInstance.h>
#include <SpriteFactory.h>
#include <Canvas.h>

#include <CameraComponent.h>

#include <JsonReader.h>

#include <Input.h>

CBootUpState::CBootUpState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myTimer(0.0f)
	, myLogoDisplayDuration(6.0f)
	, myFadeOutStart(4.5f)
	, myFadeInDuration(1.5f)
	, myWarningDisplayDuration(12.0f)
	, myLogoToRender(0)
{
}

CBootUpState::~CBootUpState()
{
}

void CBootUpState::Awake()
{
	CScene* scene = CSceneManager::CreateEmpty();

	CEngine::GetInstance()->AddScene(myState, scene);
	scene->InitCanvas();
	CEngine::GetInstance()->SetActiveScene(myState);
}

void CBootUpState::Start()
{
	rapidjson::Document document = CJsonReader::Get()->LoadDocument("Json/Settings/SplashSettings.json");
	
	CEngine::GetInstance()->SetActiveScene(myState);

	myLogos.emplace_back(new CSpriteInstance());
	myLogos.back()->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["TGA Logo Path"].GetString())));
	myLogos.back()->SetShouldRender(true);

	myLogos.emplace_back(new CSpriteInstance());
	myLogos.back()->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Group Logo Path"].GetString())));
	myLogos.back()->SetShouldRender(false);

	myLogos.emplace_back(new CSpriteInstance());
	myLogos.back()->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Engine Logo Path"].GetString())));
	myLogos.back()->SetShouldRender(false);

	myLogos.emplace_back(new CSpriteInstance());
	myLogos.back()->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Flickering Lights Warning Path"].GetString())));
	myLogos.back()->SetShouldRender(false);

	CCanvas& canvas = *IRONWROUGHT->GetActiveScene().Canvas();
	for (auto& sprite : myLogos)
	{
		canvas.AddSpriteToCanvas(sprite);
	}

	CTimer::Mark();
	myTimer = 0.0f;
	myLogoToRender = 0;

	CMainSingleton::PostMaster().SendLate({ EMessageType::BootUpState, nullptr });
}

void CBootUpState::Stop()
{
	myLogos.clear();
	CEngine::GetInstance()->RemoveScene(myState);
}

void CBootUpState::Update()
{
	myTimer += CTimer::Dt();

	if (myTimer < myFadeInDuration)
	{
		auto color = myLogos[myLogoToRender]->GetColor();
		color.x = myTimer / myFadeInDuration;
		color.y = myTimer / myFadeInDuration;
		color.z = myTimer / myFadeInDuration;
		myLogos[myLogoToRender]->SetColor(color);
	}

	if ((myTimer > myFadeOutStart) && (myLogoToRender < myLogos.size() - 1))
	{
		auto color = myLogos[myLogoToRender]->GetColor();
		color.x = 1.0f - (myTimer - myFadeOutStart) / myFadeInDuration;
		color.y = 1.0f - (myTimer - myFadeOutStart) / myFadeInDuration;
		color.z = 1.0f - (myTimer - myFadeOutStart) / myFadeInDuration;
		myLogos[myLogoToRender]->SetColor(color);
	}
	else if ((myTimer > (myWarningDisplayDuration - myFadeInDuration)) && (myLogoToRender == myLogos.size() - 1))
	{
		auto color = myLogos[myLogoToRender]->GetColor();
		color.x = 1.0f - (myTimer - (myWarningDisplayDuration - myFadeInDuration)) / myFadeInDuration;
		color.y = 1.0f - (myTimer - (myWarningDisplayDuration - myFadeInDuration)) / myFadeInDuration;
		color.z = 1.0f - (myTimer - (myWarningDisplayDuration - myFadeInDuration)) / myFadeInDuration;
		myLogos[myLogoToRender]->SetColor(color);
	}

	if ((myTimer > myLogoDisplayDuration) && (myLogoToRender < myLogos.size() - 1)) {
		myTimer -= myLogoDisplayDuration;

		myLogos[myLogoToRender]->SetShouldRender(false);
		myLogoToRender++;

		if (myLogoToRender >= myLogos.size())
		{
			IRONWROUGHT->ShowCursor(false);
			myStateStack.PopTopAndPush(CStateStack::EState::InGame);
			return;
		}

		myLogos[myLogoToRender]->SetShouldRender(true);
		auto color = myLogos[myLogoToRender]->GetColor();
		color.x = myTimer / myFadeInDuration;
		color.y = myTimer / myFadeInDuration;
		color.z = myTimer / myFadeInDuration;
		myLogos[myLogoToRender]->SetColor(color);
	}
	else if ((myTimer > myWarningDisplayDuration) && (myLogoToRender == myLogos.size() - 1))
	{
		myLogos[myLogoToRender]->SetShouldRender(false);
		myLogoToRender++;

		if (myLogoToRender >= myLogos.size())
		{
			IRONWROUGHT->ShowCursor(false);
			myStateStack.PopTopAndPush(CStateStack::EState::InGame);
			return;
		}
	}

#ifdef _DEBUG
	if (INPUT->IsKeyPressed(VK_SPACE))
	{
		myLogoToRender++;

		if (myLogoToRender >= myLogos.size())
		{
			myStateStack.PopTopAndPush(CStateStack::EState::InGame);
			return;
		}
		myTimer = 0.0f;

		myLogos[myLogoToRender]->SetShouldRender(false);
		myLogoToRender++;

		if (myLogoToRender >= myLogos.size())
		{
			myStateStack.PopTopAndPush(CStateStack::EState::InGame);
			return;
		}

		myLogos[myLogoToRender]->SetShouldRender(true);
		auto color = myLogos[myLogoToRender]->GetColor();
		color.x = myTimer / myFadeInDuration;
		color.y = myTimer / myFadeInDuration;
		color.z = myTimer / myFadeInDuration;
		myLogos[myLogoToRender]->SetColor(color);
	}
#endif
}

void CBootUpState::Receive(const SStringMessage& /*aMessage*/)
{
}

void CBootUpState::Receive(const SMessage& /*aMessage*/)
{
}
