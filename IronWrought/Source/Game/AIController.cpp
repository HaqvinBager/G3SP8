#include "stdafx.h"
#include "TransformComponent.h"
#include "AIController.h"
#include "PhysXWrapper.h"
#include "PlayerComponent.h"
#include "LineInstance.h"
#include "LineFactory.h"
#include "Scene.h"
#include "NavmeshLoader.h"
#include <AStar.h>
#include <Debug.h>
#include "EnemyComponent.h"
#include "PatrolPointComponent.h"

bool CheckIfOverlap(const Vector3& aFirstPosition, const Vector3& aSecondPosition)
{
	float xDifference = fabs(aFirstPosition.x - aSecondPosition.x);
	float zDifference = fabs(aFirstPosition.z - aSecondPosition.z);
	if (xDifference > 0.1f)
		return false;
	if (zDifference > 0.1f)
		return false;
	return true;
}

CPatrol::CPatrol(const std::vector<CPatrolPointComponent*>& somePositions, SNavMesh* aNavMesh)
{
	myPatrolPoints = somePositions;
	myTarget = 0;
	myPathTarget = 0;
	myNavMesh = aNavMesh;
}

void CPatrol::Enter(const Vector3& aPosition)
{
	myPath.clear();
	if (myNavMesh == nullptr)
		return;

	if (myPatrolPoints.empty())
		return;

	CPatrolPointComponent* patrolPoint = myPatrolPoints[myTarget];
	if (patrolPoint != nullptr) {
		SetPath(myNavMesh->CalculatePath(aPosition, patrolPoint->GameObject().myTransform->Position(), myNavMesh), patrolPoint->GameObject().myTransform->Position());
	}
}

Vector3 CPatrol::Update(const Vector3& aPosition)
{
	if (myNavMesh == nullptr)
		return Vector3::Zero;
	if (myPatrolPoints.empty())
		return Vector3::Zero;

	Vector3 patrolPointPosition = myPatrolPoints[myTarget]->GameObject().myTransform->Position();
	CPatrolPointComponent* patrolPoint;// = FindBestPatrolPoint(aPosition);

	if (CheckIfOverlap(aPosition, patrolPointPosition)) // change patrol points & calculate path
	{
		myPatrolPoints[myTarget]->AddBonusValue(5);
		patrolPoint = FindBestPatrolPoint(aPosition);
		for (int i = 0; i < myPatrolPoints.size(); ++i) {
			if (myPatrolPoints[i] == patrolPoint) {
				myTarget = i;
				patrolPointPosition = myPatrolPoints[myTarget]->GameObject().myTransform->Position();
			}
		}
		CMainSingleton::PostMaster().Send({EMessageType::EnemyReachedTarget});
		//SetPath(myNavMesh->CalculatePath(aPosition, patrolPointPosition, myNavMesh), patrolPointPosition);
	}

	size_t pathSize = myPath.size();
	if (pathSize > 0) {
		Vector3 newPos;
		Vector3 dir;

		dir = (myPath[pathSize - 1] - aPosition);
		dir.Normalize();

		if (CheckIfOverlap(aPosition, myPath[pathSize - 1])) {
			myPath.pop_back();
		}
		return dir;
	}
	return Vector3();
}

void CPatrol::ClearPath() {
	myPath.clear();
}

void CPatrol::SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition)
{
	if (aPath.empty()) {
		return;
	}

	myPath.clear();
  	myPath.push_back(aFinalPosition);
	for (unsigned int i = 1; i < aPath.size(); ++i) {
		if (aPath[i] != aFinalPosition) {
			myPath.push_back(aPath[i]);
			CDebug::GetInstance()->DrawLine(aPath[i - 1], aPath[i], 60.0f);
		}
	}
}

CPatrolPointComponent* CPatrol::FindBestPatrolPoint(const Vector3& aPosition)
{
	//const std::vector<CPatrolPointComponent*>& patrolPoints = CEngine::GetInstance()->GetActiveScene().PatrolPoints();
	std::vector<float> intrestValues;
	if (!myPatrolPoints.empty()) {
		for (int i = 0; i < myPatrolPoints.size(); ++i) {
			Vector3 patrolPositions = myPatrolPoints[i]->GameObject().myTransform->Position();
			Vector3 dist = patrolPositions - aPosition;
			float length = dist.LengthSquared() / 10.f;
			myPatrolPoints[i]->AddValue(length);
			intrestValues.push_back(myPatrolPoints[i]->GetIntrestValue());
			//std::cout << "[" << i << "] " << "Length Value: " << patrolPoints[i]->GetIntrestValue() << std::endl;
		}

		float min = *std::min_element(intrestValues.begin(), intrestValues.end());
		//std::cout << "Length Value: " << min << std::endl;

		for (int i = 0; i < myPatrolPoints.size(); ++i) {
			if (myPatrolPoints[i]->GetIntrestValue() <= min) {
				return myPatrolPoints[i];
			}
		}
	}
	return nullptr;
}

CSeek::CSeek(SNavMesh* aNavMesh) : myNavMesh(aNavMesh), myTarget(nullptr) {
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyFoundPlayer, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyLostPlayer, this);
}

CSeek::~CSeek()
{
	myTarget = nullptr;
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyFoundPlayer, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyLostPlayer, this);
}

void CSeek::Enter(const Vector3& aPosition)
{
	myPath.clear();

	aPosition;
}

Vector3 CSeek::Update(const Vector3& aPosition)//aPostion == EnemyRobot Position
{
	if (!myTarget)
		return Vector3();

	myPathTarget = 0;
	float epsilon = 0.5f;
	if (myFoundPlayer == true) {
		SetPath(myNavMesh->CalculatePath(aPosition, myTarget->Position(), myNavMesh), myTarget->Position());
	}
	else {
		float dist = DirectX::SimpleMath::Vector2::DistanceSquared({ myLastPlayerPosition.x, myLastPlayerPosition.z }, { aPosition.x, aPosition.z });
		SetPath(myNavMesh->CalculatePath(aPosition, myLastPlayerPosition, myNavMesh), myLastPlayerPosition);
		if (dist < epsilon) {
			CMainSingleton::PostMaster().Send({ EMessageType::EnemyReachedLastPlayerPosition });
		}
	}

	size_t pathSize = myPath.size();
	if (pathSize > 0) {

		Vector3 newPos;
		Vector3 dir;

		/*float*/ epsilon = 0.05f;

		dir = (myPath[pathSize - 1] - aPosition);
		dir.Normalize();

		if (DirectX::SimpleMath::Vector3::DistanceSquared(aPosition, myPath[pathSize - 1]) < epsilon) {
			myPath.pop_back();
		}
		return dir;
	}
	return Vector3();
}

void CSeek::ClearPath() {
	myPath.clear();
}

void CSeek::SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition)
{
	if (aPath.empty()) {
		return;
	}

	myPath.clear();
	myPath.push_back(aFinalPosition);
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		myPath.push_back(aPath[i]);
	}
}

void CSeek::SetTarget(CTransformComponent* aTarget) {
	myTarget = aTarget;
}

void CSeek::Receive(const SMessage& aMsg)
{
	if (aMsg.myMessageType == EMessageType::EnemyFoundPlayer) {
		myFoundPlayer = true;
	}

	if (aMsg.myMessageType == EMessageType::EnemyLostPlayer) {
		myFoundPlayer = false;
		myLastPlayerPosition = *static_cast<Vector3*>(aMsg.data);
	}
}

CAttack::CAttack(CEnemyComponent* aUser, Vector3 aResetPosition) : myDamage(1.0f), myTarget(nullptr), myAttackCooldown(1.f), myAttackTimer(0.f), myUser(aUser), myResetPosition(aResetPosition) {}

void CAttack::Enter(const Vector3& aPosition)
{
	myPath.clear();

	aPosition;
}

Vector3 CAttack::Update(const Vector3& aPosition)
{

	if (!myTarget) {
		return Vector3();
	}
	Vector3 direction = myTarget->WorldPosition() - aPosition;

	//byt ut attacktimer och attackcooldown till animationtimer - Alexander Matth�i 2021-05-07
	myAttackTimer += CTimer::Dt();
	if (myAttackTimer >= myAttackCooldown) {
		Vector3 origin = aPosition;
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 10.0f, CPhysXWrapper::ELayerMask::PLAYER);
		int hits = hit.getNbAnyHits();

		/*CLineInstance* myLine2 = new CLineInstance();
		myLine2->Init(CLineFactory::GetInstance()->CreateLine(origin, origin + (direction * 10.f), { 255,0,0,255 }));
		CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);*/

		if (hits > 0) {
			//std::cout << __FUNCTION__ <<  " Player Hit " << std::endl;
			myUser->GameObject().myTransform->PositionRigidbody(myResetPosition);
			CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyAttack, myUser });
			CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage });
			CMainSingleton::PostMaster().Send({ EMessageType::EnemyAttackedPlayer });
		}
		myAttackTimer = 0.f;
	}
	return std::move(direction);
}

void CAttack::ClearPath() {
	myPath.clear();
}

void CAttack::SetTarget(CTransformComponent* aTarget)
{
	myTarget = aTarget;
}

CAlerted::CAlerted(SNavMesh* aNavMesh)
{
	myNavMesh = aNavMesh;
}

void CAlerted::Enter(const Vector3& aPosition)
{

	myPath.clear();

	aPosition;
}

Vector3 CAlerted::Update(const Vector3& aPosition)
{

	SetPath(myNavMesh->CalculatePath(aPosition, myAlertedPosition, myNavMesh), myAlertedPosition);

	float dist = DirectX::SimpleMath::Vector3::DistanceSquared(aPosition, myAlertedPosition);
	float epsilon = 0.3f;
	if (dist < epsilon) {
		CMainSingleton::PostMaster().Send({ EMessageType::EnemyReachedTarget });
	}

	size_t pathSize = myPath.size();
	if (pathSize > 0) {

		Vector3 newPos;
		Vector3 dir;

		/*float*/ epsilon = 0.05f;

		dir = (myPath[pathSize - 1] - aPosition);
		dir.Normalize();

		if (DirectX::SimpleMath::Vector3::DistanceSquared(aPosition, myPath[pathSize - 1]) < epsilon) {
			myPath.pop_back();
		}
		return dir;
	}
	return Vector3();
}

void CAlerted::ClearPath()
{
}

void CAlerted::SetAlertedPosition(const Vector3& aAlertedPosition)
{
	myAlertedPosition = aAlertedPosition;
}

void CAlerted::SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition)
{
	if (aPath.empty()) {
		return;
	}

	myPath.clear();
	myPath.push_back(aFinalPosition);
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		myPath.push_back(aPath[i]);
	}
}

CIdle::CIdle()
{
}

void CIdle::Enter(const Vector3& /*aPosition*/)
{
}

Vector3 CIdle::Update(const Vector3& /*aPosition*/)
{
	return Vector3::Zero;
}

void CIdle::ClearPath()
{
}