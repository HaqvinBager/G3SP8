#include "stdafx.h"
#include "PhysicsPropAudioComponent.h"
#include "Engine.h"
#include "AudioChannel.h"

CPhysicsPropAudioComponent::CPhysicsPropAudioComponent(CGameObject& aParent, const PostMaster::SAudioSourceInitData& someSettings)
    : CComponent(aParent)
    , mySettings(someSettings)
    , myTimer(0.0f)
{
    myAudioChannel = CEngine::GetInstance()->RequestAudioSource(mySettings);
    myPlayMessage.myChannel = myAudioChannel;
    myPlayMessage.mySoundIndex = mySettings.mySoundIndex;
}

CPhysicsPropAudioComponent::~CPhysicsPropAudioComponent()
{
}

void CPhysicsPropAudioComponent::Awake()
{
}

void CPhysicsPropAudioComponent::Start()
{
}

void CPhysicsPropAudioComponent::Update()
{
    myTimer += CTimer::Dt();
}

bool CPhysicsPropAudioComponent::Ready()
{
    if (myTimer > 0.2f)
    {
        myTimer = 0.0f;
        return true;
    }
    return false;
}

PostMaster::SPlayDynamicAudioData& CPhysicsPropAudioComponent::GetPlayMessage()
{
    const Matrix& matrix = GameObject().myTransform->GetWorldMatrix();
    myAudioChannel->Set3DAttributes(matrix.Translation());
    return myPlayMessage;
}
