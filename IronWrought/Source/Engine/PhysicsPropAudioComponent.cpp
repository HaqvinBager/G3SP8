#include "stdafx.h"
#include "PhysicsPropAudioComponent.h"

CPhysicsPropAudioComponent::CPhysicsPropAudioComponent(CGameObject& aParent, unsigned int aSoundIndex)
    : CComponent(aParent)
    , mySoundIndex(aSoundIndex)
    , myTimer(0.0f)
{
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

unsigned int& CPhysicsPropAudioComponent::GetSoundIndex()
{
    return mySoundIndex;
}
