#include "stdafx.h"
#include "DecalComponent.h"
#include "ModelComponent.h"
#include "DecalFactory.h"
#include "GameObject.h"
#include "Decal.h"

//CDecalComponent::CDecalComponent(CGameObject& aParent, const std::string& aFBXPath)
//    : CBehavior(aParent)
//{
//    myDecal = CDecalFactory::GetInstance()->GetDecal(aFBXPath);
//    myDecalPath = aFBXPath;
//    SetAlphaThreshold(0.3f);
//}

CDecalComponent::CDecalComponent(CGameObject& aParent, const SSettings& someSettings)
    : CBehavior(aParent)
    , mySettings(someSettings)  
{
    myDecal = CDecalFactory::GetInstance()->GetDecal(mySettings.myFBXPath); 
    myDecal->RenderAlbedo(someSettings.myShouldRenderAlbedo == 1 ? true : false);
    myDecal->RenderAlbedo(someSettings.myShouldRenderMaterial == 1 ? true : false);
    myDecal->RenderAlbedo(someSettings.myShouldRenderNormals == 1 ? true : false);
    SetAlphaThreshold(0.3f);
}

CDecalComponent::~CDecalComponent()
{
    CDecalFactory::GetInstance()->ClearDecal(mySettings.myFBXPath);
}

void CDecalComponent::Awake()
{
}

void CDecalComponent::Start()
{
}

void CDecalComponent::Update()
{
}

CDecal* CDecalComponent::GetMyDecal() const
{
    return myDecal;
}


void CDecalComponent::SetAlphaThreshold(float aThreshold)
{
    myDecal->GetDecalData().myAlphaClipThreshold = aThreshold;
}

void CDecalComponent::OnEnable()
{
}

void CDecalComponent::OnDisable()
{
}
