#include "stdafx.h"
#include "KeyBehavior.h"

CKeyBehavior::CKeyBehavior(CGameObject& aParent, const SSettings& someSettings) : CBehavior(aParent), mySettings(someSettings)
{
	CMainSingleton::PostMaster().Send({ mySettings.myOnCreateNotify.c_str(), mySettings.myData });
}

CKeyBehavior::~CKeyBehavior()
{
}

void CKeyBehavior::Awake()
{
}

void CKeyBehavior::Start()
{
}

void CKeyBehavior::Update()
{
}

void CKeyBehavior::OnEnable()
{
}

void CKeyBehavior::OnDisable()
{
}
