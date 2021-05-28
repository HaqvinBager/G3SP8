#include "stdafx.h"
#include "AnimateKey.h"

CAnimateKey::CAnimateKey(CGameObject& aParent, const SSettings& someSettings) : CKeyBehavior(aParent, someSettings)
{
}

CAnimateKey::~CAnimateKey()
{
}

void CAnimateKey::OnInteract()
{
	CMainSingleton::PostMaster().Send({ mySettings.myInteractNotify.c_str(), mySettings.myData });
	//do animation stuff I guess
}