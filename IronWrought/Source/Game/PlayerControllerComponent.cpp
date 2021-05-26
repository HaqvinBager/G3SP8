#include "stdafx.h"
#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

#include "InputMapper.h"

#include "PhysXWrapper.h"
#include "CharacterController.h"

#include "Scene.h"
#include "CameraControllerComponent.h"

#include "PlayerAnimationController.h"
#include "PlayerComponent.h"
#include "ModelComponent.h"

#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

#define PLAYER_MAX_POSITION 500.0f
#define PLAYER_MIN_POSITION -500.0f

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed, const float aCrouchSpeed, physx::PxUserControllerHitReport* aHitReport)
	: CComponent(gameObject)
	, mySpeed(aWalkSpeed)
	, myIsCrouching(false)
	, myWalkSpeed(aWalkSpeed)
	, myCrouchSpeed(aCrouchSpeed)
	, myIsGrounded(true)
	, myHasJumped(false)
	, myIsJumping(false)
	, myJumpHeight(0.07f)// these values don't make sense. //Supposed to be 40cm => ~0.4f
	, myFallSpeed(0.982f * 1.0f)
	, myMovement( Vector3(0.0f, 0.0f, 0.0f ))
	, myAirborneTimer(0.0f)
	, myLadderHasTriggered(false)
	, myAnimationComponentController(nullptr)
	, myPlayerComponent(nullptr)
	, myCanStand(true)
	, myCrouchingLerp(0.0f)
	, myStepTimer(0.0f)
	, myStepTime(aWalkSpeed * 5.0f)
	, myMovementLockTimer(0.0f)
	, myCurrentFloorMaterial(EFloorMaterial::Default)
{
	INPUT_MAPPER->AddObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Crouch, this);
	INPUT_MAPPER->AddObserver(EInputEvent::ResetEntities, this);
	INPUT_MAPPER->AddObserver(EInputEvent::SetResetPointEntities, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerTakeDamage, this);

	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(gameObject.myTransform->Position(), myColliderRadius, myColliderHeightStanding, GameObject().myTransform, aHitReport);
	physx::PxShape* shape = nullptr;
	myController->GetController().getActor()->getShapes(&shape, 1);
	shape->setFlag(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
	
	PxFilterData filterData;
	filterData.word0 = CPhysXWrapper::ELayerMask::PLAYER;
	shape->setQueryFilterData(filterData);

	GameObject().myTransform->Position(myController->GetPosition());// This is a test / Aki 2021 03 12

	GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });
	GameObject().myTransform->FetchChildren()[0]->Rotation({ 0.0f, 0.0f, 0.0f });
	myCamera = GameObject().myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraControllerComponent>();
	//CAnimationComponent* animComp = GameObject().myTransform->FetchChildren()[0]->GameObject().GetComponent<CAnimationComponent>();
	//if (animComp)
	//{
	//	myAnimationComponentController = new CPlayerAnimationController();
	//	myAnimationComponentController->Init(animComp);
	//}
	//else
	//{
	//	assert(false && "No animation component available!");
	//}
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	INPUT_MAPPER->RemoveObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Crouch, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::ResetEntities, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::SetResetPointEntities, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerTakeDamage, this);

	delete myAnimationComponentController;
	myAnimationComponentController = nullptr;
}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{
	SetRespawnPosition();
}

void CPlayerControllerComponent::Update()
{
#ifdef _DEBUG
	if (myCamera->IsFreeCamMode() || myCamera->IsCursorUnlocked())
	{
	/*	CGameObject* playerGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(-28554);
		if(myController != nullptr && playerGameObject != nullptr)
			myController->SetPosition(playerGameObject->myTransform->WorldPosition());*/

		return;
	}

	//if (INPUT->IsKeyPressed('M'))
	//{
	//	LockMovementFor(3.0f);
	//}
#endif

	GameObject().myTransform->Position(myController->GetPosition());
	//myAnimationComponentController->Update(myMovement);

	switch (myPlayerMovementLock)
	{
		case EPlayerMovementLock::None:
		{
			ControllerUpdate();
		}break;

		case EPlayerMovementLock::ForceFoward:
		{
			UpdateForceForward();
		}break;

		case EPlayerMovementLock::ForceStandStill:
		{
			UpdateStandStill();
		}break;

		default:break;
	}

	UpdateMovementLock();
	BoundsCheck();


#ifdef _DEBUG
	if (Input::GetInstance()->IsKeyPressed('R'))
	{
		ResetPlayerPosition();
	}
#endif // _DEBUG
}

void CPlayerControllerComponent::FixedUpdate()
{
	if (myLadderHasTriggered)
	{
		LadderUpdate();
	}
	else
	{
		if (myHasJumped == true)
		{
			myMovement.y = myJumpHeight;
			myAirborneTimer = 0.0f;
			myHasJumped = false;
			CMainSingleton::PostMaster().SendLate({ EMessageType::PlayJumpSound, nullptr });
		}

		myMovement.y -= myFallSpeed * myFallSpeed * CTimer::FixedDt() * myAirborneTimer ;
		if (!myIsGrounded) {
			myAirborneTimer += CTimer::FixedDt();
		}

		if (myMovement.y < myMaxFallSpeed)
			myMovement.y = myMaxFallSpeed;

		Move({ myMovement.x, myMovement.y, myMovement.z });
		CrouchUpdate(CTimer::FixedDt() * 2.0f);
	}
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
switch (myPlayerMovementLock)
	{
		case EPlayerMovementLock::None:
		{}break;

		case EPlayerMovementLock::ForceFoward:
		{
			InitForceForward();
			return;
		}break;

		case EPlayerMovementLock::ForceStandStill:
		{
			return;
		}break;

		default:break;
	}

#ifdef _DEBUG
	if (myCamera->IsFreeCamMode() || myCamera->IsCursorUnlocked())
	{
		if (aEvent == EInputEvent::SetResetPointEntities)
			myRespawnPosition = myCamera->GameObject().myTransform->WorldPosition();

		return;
	}
#endif

	float y = myMovement.y;

	switch (aEvent)
	{
		case EInputEvent::Jump:
			if (myIsGrounded == true)
			{
				myHasJumped = true;
				myIsJumping = true;
				myIsGrounded = false;
			}
			break;
		case EInputEvent::Crouch:
			/*Crouch();*/
			OnCrouch();
			break;

		case EInputEvent::ResetEntities:
			myController->SetPosition(myRespawnPosition);
			GameObject().myTransform->Position(myController->GetPosition());
			break;

		case EInputEvent::SetResetPointEntities:
			myRespawnPosition = myController->GetPosition();
			break;

		default:break;
	}

	myMovement.y = y;
}

void CPlayerControllerComponent::Receive(const SStringMessage& /*aMsg*/)
{
}

void CPlayerControllerComponent::Receive(const SMessage& aMsg)
{
	if (aMsg.myMessageType == EMessageType::PlayerTakeDamage)
	{
		//myAnimationComponentController->TakeDamage();
	}
}

void CPlayerControllerComponent::ControllerUpdate()
{
	const float horizontalInput = Input::GetInstance()->GetAxis(Input::EAxis::Horizontal);
	const float verticalInput = Input::GetInstance()->GetAxis(Input::EAxis::Vertical);
	Vector3 horizontal = -GameObject().myTransform->GetLocalMatrix().Right() * horizontalInput;
	Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward() * verticalInput;
	float y = myMovement.y;
	myMovement = (horizontal + vertical) * mySpeed;
	myMovement.y = y;
}

void CPlayerControllerComponent::Move(Vector3 aDir)
{
	physx::PxControllerCollisionFlags collisionflag = myController->GetController().move({ aDir.x, aDir.y, aDir.z}, 0, CTimer::FixedDt(), 0);
	
	if (!myIsGrounded && (collisionflag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)) 
	{
		CMainSingleton::PostMaster().SendLate({ EMessageType::PlayStepSound, &myCurrentFloorMaterial }); // Landing
	}
	
	myIsGrounded = (collisionflag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);
	if (myIsGrounded)
	{
		myAirborneTimer = 0.f;

		Vector2 horizontalDir(aDir.x, aDir.z);
		if (horizontalDir.LengthSquared() > 0.0f)
		{
			myStepTimer -= CTimer::FixedDt();
			if (myStepTimer <= 0.0f && !myIsCrouching) 
			{
				myStepTimer = myStepTime;
				CMainSingleton::PostMaster().SendLate({ EMessageType::PlayStepSound, &myCurrentFloorMaterial });
			}
		}
	}
}

void CPlayerControllerComponent::SetControllerPosition(const Vector3& aPos)
{
	myController->GetController().setPosition({ aPos.x, aPos.y, aPos.x });
}

inline const float Lerp(const float& A, const float& B, const float& T)
{
	return ((A - A * T) + (B * T));
}

void CPlayerControllerComponent::Crouch()
{
	myIsCrouching = !myIsCrouching;
	if (myIsCrouching)
	{
		myController->GetController().resize(myColliderHeightCrouched);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYCrouching, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
		mySpeed = myCrouchSpeed;
		// THIS IS TEMP :)
		//myAnimationComponentController->TakeDamage();// TEMP :)
		// SUPER TEMP :)
	}
	else
	{
		myController->GetController().resize(myColliderHeightStanding);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
		mySpeed = myWalkSpeed;
	}
}

void CPlayerControllerComponent::CrouchUpdate(const float& dt)
{
	if (myCrouchingLerp >= 0.0f && myCrouchingLerp <= 1.0f)
	{
		float currentYPos = Lerp(myCameraPosYStanding, myCameraPosYCrouching, myCrouchingLerp);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, currentYPos, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
	}
	else
	{
		return;
	}

	if(myIsCrouching && myCanStand)
		myCrouchingLerp += dt * 2.0f;
	else
		myCrouchingLerp -= dt * 2.0f;
}

void CPlayerControllerComponent::OnCrouch()
{
if (myCanStand) {
		myIsCrouching = !myIsCrouching;
	}
	if (myIsCrouching)
	{
		myController->GetController().resize(myColliderHeightCrouched);
		mySpeed = myCrouchSpeed;
		myCrouchingLerp = 0.0f;
	}
	else
	{
		Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
		start.y += (myColliderHeightStanding / 2);
		Vector3 dir = GameObject().myTransform->GetWorldMatrix().Up();
		//checks if we can stand up 
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, (myColliderHeightStanding), CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT);
		if (hit.getNbAnyHits() <= 0) {
			myCanStand = true;
			myController->GetController().resize(myColliderHeightStanding);
			mySpeed = myWalkSpeed;
			myCrouchingLerp = 1.0f;
		}
		else {
			myCanStand = false;
		}
	}
}

void CPlayerControllerComponent::ResetPlayerPosition()
{
	myController->SetPosition(myRespawnPosition);
}

CCharacterController* CPlayerControllerComponent::GetCharacterController()
{
	return myController;
}

const Vector3 CPlayerControllerComponent::GetLinearVelocity()
{
	const PxVec3 pxVec3 = myController->GetController().getActor()->getLinearVelocity();
	return {pxVec3.x, pxVec3.y, pxVec3.z};
}

void CPlayerControllerComponent::LadderEnter()
{
	myLadderHasTriggered = true;
}

void CPlayerControllerComponent::LadderExit()
{
	myLadderHasTriggered = false;
}

void CPlayerControllerComponent::SetRespawnPosition()
{
	myRespawnPosition = myController->GetPosition();
}

void CPlayerControllerComponent::SetCurrentFloorMaterial(const std::string& anObjectTag)
{
	if (anObjectTag == "WoodFloor")
		myCurrentFloorMaterial = EFloorMaterial::WoodFloor;

	else if (anObjectTag == "Carpet")
		myCurrentFloorMaterial = EFloorMaterial::Carpet;

	else
		myCurrentFloorMaterial = EFloorMaterial::Default;
}

void CPlayerControllerComponent::LockMovementFor(const float& someSeconds)
{
	myMovementLockTimer = someSeconds;
	myMovement = { 0.0f, myMovement.y, 0.0f };
}

void CPlayerControllerComponent::UpdateMovementLock()
{
	if (myMovementLockTimer < 0.0f)
		return;

	myMovementLockTimer -= CTimer::Dt();
	myMovement = { 0.0f, myMovement.y, 0.0f };
}

void CPlayerControllerComponent::InitForceForward()
{
	Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward();
	float y = myMovement.y;
	myMovement = (vertical) * mySpeed;
	myMovement.y = y;
}

void CPlayerControllerComponent::UpdateForceForward()
{
	const float horizontalInput = Input::GetInstance()->GetAxis(Input::EAxis::Horizontal);
	const float verticalInput = Input::GetInstance()->GetAxis(Input::EAxis::Vertical);

	if (horizontalInput != 0.0f || verticalInput != 0.0f)
	{
		Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward();
		float y = myMovement.y;
		myMovement = (vertical) * mySpeed;
		myMovement.y = y;
	}
	else
	{
		Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward();
		float y = myMovement.y;
		myMovement = (vertical) * mySpeed; //* 0.5f;
		myMovement.y = y;
	}
}

void CPlayerControllerComponent::InitStandStill(const float& aStandStillTimer)
{
	LockMovementFor(aStandStillTimer);
	myPlayerMovementLock = EPlayerMovementLock::ForceStandStill;
}

void CPlayerControllerComponent::UpdateStandStill()
{
	if (myMovementLockTimer <= 0.0f)
		myPlayerMovementLock = EPlayerMovementLock::None;
}

void CPlayerControllerComponent::BoundsCheck()
{
	const Vector3 playerPos = GameObject().myTransform->Position();
	if ((playerPos.y < PLAYER_MAX_POSITION && playerPos.y > PLAYER_MIN_POSITION) == false)
		ResetPlayerPosition();
}

void CPlayerControllerComponent::LadderUpdate()
{
	if (Input::GetInstance()->IsKeyDown('W'))
	{
		Move({ 0.0f, 0.075f, 0.0f });
	}
	else if (Input::GetInstance()->IsKeyDown('S'))
	{
		Move({ 0.0f, -0.075f, 0.0f });
	}

	if (myIsGrounded)
	{
		if (Input::GetInstance()->IsKeyDown('S'))
		{
			LadderExit();
		}
	}
}