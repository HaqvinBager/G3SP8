#include "Button.h"
#include "stdafx.h"
#include "MainSingleton.h"
#include "SpriteFactory.h"
#include "SpriteInstance.h"
#include "Sprite.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "Scene.h"
#include "PostMaster.h"

void CButton::OnHover()
{
	myState = EButtonState::Hover;
	mySprites.at(static_cast<size_t>(EButtonState::Idle))->SetShouldRender(false);
	mySprites.at(static_cast<size_t>(EButtonState::Hover))->SetShouldRender(true);
}

void CButton::OnClickDown()
{	
	myState = EButtonState::Click;
	mySprites.at(static_cast<size_t>(EButtonState::Hover))->SetShouldRender(false);
	mySprites.at(static_cast<size_t>(EButtonState::Click))->SetShouldRender(true);

	SMessage msg = { EMessageType::UIButtonPress, nullptr };
	CMainSingleton::PostMaster().Send(msg);
}

void CButton::OnClickUp(void* someData)
{
	myState = EButtonState::Hover;
	mySprites.at(static_cast<size_t>(EButtonState::Click))->SetShouldRender(false);
	mySprites.at(static_cast<size_t>(EButtonState::Hover))->SetShouldRender(true);

	for (unsigned int i = 0; i < myMessagesToSend.size(); ++i)
	{
		CMainSingleton::PostMaster().SendLate({ myMessagesToSend[i], someData });
	}

	if (myWidgetToToggleIndex > -1)
	{
		CMainSingleton::PostMaster().SendLate({ EMessageType::ToggleWidget, &myWidgetToToggleIndex });
	}
}

void CButton::OnLeave()
{
	myState = EButtonState::Idle;
	mySprites.at(static_cast<size_t>(EButtonState::Click))->SetShouldRender(false);
	mySprites.at(static_cast<size_t>(EButtonState::Hover))->SetShouldRender(false);
	mySprites.at(static_cast<size_t>(EButtonState::Idle))->SetShouldRender(true);
}

bool CButton::Click(bool anIsPressed, void* someData)
{
	if (!myEnabled)
		return false;

	switch (myState)
	{
	case EButtonState::Hover:
		if (anIsPressed && myIsMouseHover) {
			OnClickDown();
		}
		break;
	case EButtonState::Click:
		if (!anIsPressed && myIsMouseHover) {
			OnClickUp(someData);
			return true;
		}
		else if (!myIsMouseHover) {
			OnLeave();
		}
		break;
	default:
		break;
	}

	return false;
}

void CButton::CheckMouseCollision(DirectX::SimpleMath::Vector2 aScreenSpacePosition)
{
	if (!myEnabled)
		return;

	if 
		(!(
			myRect.myBottom < aScreenSpacePosition.y ||
			aScreenSpacePosition.y < myRect.myTop    ||
			myRect.myRight < aScreenSpacePosition.x  ||
			aScreenSpacePosition.x < myRect.myLeft
		)) 
	{
		myIsMouseHover = true;

		switch (myState)
		{
		case EButtonState::Idle:
			OnHover();
			break;
		default:
			break;
		}
	}
	else {
		myIsMouseHover = false;

		switch (myState)
		{
		case EButtonState::Hover:
			OnLeave();
			break;
		case EButtonState::Click:
			OnLeave();
			break;
		default:
			break;
		}
	}
}
void CButton::Enabled(const bool& anIsEnabled)
{
	if (myEnabled != anIsEnabled)
	{
		myEnabled = anIsEnabled;
		for (int i = 0; i < mySprites.size(); ++i)
		{
			if (anIsEnabled == true && static_cast<EButtonState>(i) != myState)
				continue;
			mySprites[i]->SetShouldRender(myEnabled);
		}
	}
}

const bool CButton::Enabled()
{
	return myEnabled;
}

void CButton::SetRenderLayer(const ERenderOrder& aRenderLayer)
{
	mySprites[0]->SetRenderOrder(aRenderLayer);
	mySprites[1]->SetRenderOrder(aRenderLayer);
	mySprites[2]->SetRenderOrder(aRenderLayer);
}

const ERenderOrder CButton::GetRenderLayer()
{
	return mySprites[0]->GetRenderOrder();
}

CButton::CButton()
	: myState(EButtonState::Idle)
	, myEnabled(false)
	, myIsMouseHover(false)
	, myWidgetToToggleIndex(-1)
{}

CButton::CButton(SButtonData& someData)
{
	Init(someData);
}

CButton::~CButton()
{
	delete mySprites.at(static_cast<size_t>(EButtonState::Idle));
	delete mySprites.at(static_cast<size_t>(EButtonState::Hover));
	delete mySprites.at(static_cast<size_t>(EButtonState::Click));
}

void CButton::Init(SButtonData& someData)
{
	myMessagesToSend = someData.myMessagesToSend;
	myState = EButtonState::Idle;
	myEnabled = true;
	myIsMouseHover = false;

	CSpriteFactory& spriteFactory = *CSpriteFactory::GetInstance();
	for (unsigned int i = 0; i < mySprites.max_size(); ++i) 
	{
		if (mySprites.at(i) == nullptr)
		{
			mySprites.at(i) = new CSpriteInstance();
		}

		mySprites.at(i)->Init(spriteFactory.GetSprite(someData.mySpritePaths.at(i)));
		mySprites.at(i)->SetRenderOrder(ERenderOrder::Layer2);
		mySprites.at(i)->SetPosition({ someData.myPosition.x, someData.myPosition.y });
		
	}
	mySprites.at(static_cast<size_t>(EButtonState::Hover))->SetShouldRender(false);
	mySprites.at(static_cast<size_t>(EButtonState::Click))->SetShouldRender(false);

	float windowWidth = CEngine::GetInstance()->GetWindowHandler()->GetResolution().x;
	float windowHeight = CEngine::GetInstance()->GetWindowHandler()->GetResolution().y;

	DirectX::SimpleMath::Vector2 normalizedPosition = someData.myPosition;
	normalizedPosition /= 2.0f;
	normalizedPosition += { 0.5f, 0.5f };

	DirectX::SimpleMath::Vector2 spriteDimensions = someData.myDimensions;
	spriteDimensions /= 2.0f;
	myRect.myTop = normalizedPosition.y * windowHeight - spriteDimensions.y;
	myRect.myBottom = normalizedPosition.y * windowHeight + spriteDimensions.y;
	myRect.myLeft = normalizedPosition.x * windowWidth - spriteDimensions.x;
	myRect.myRight = normalizedPosition.x * windowWidth + spriteDimensions.x;

	myWidgetToToggleIndex = someData.myWidgetToToggleIndex;
}
