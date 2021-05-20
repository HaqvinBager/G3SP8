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

CPatrol::CPatrol(const std::vector<Vector3>& somePositions, SNavMesh* aNavMesh)
{
	myPositions = somePositions;
	myTarget = 0;
	myPathTarget = 0;
	myNavMesh = aNavMesh;
}

Vector3 CPatrol::Update(const Vector3& aPosition)
{
	if (myPositions.empty())
		return Vector3::Zero;

	if (CheckIfOverlap(aPosition, myPositions[myTarget])) // change patrol points & calculate path
	{
		myLastTarget = myTarget;
		myTarget++;

		if (myTarget >= myPositions.size())
		{
			myTarget = 0;
		}

		myPathTarget = 0;
		myPath = CAStar::GetInstance()->GetPath(myPositions[myLastTarget], myPositions[myTarget], myNavMesh);
	}

	if (!myPath.empty()) {
		if (myPathTarget < myPath.size()) {
			if (CheckIfOverlap(aPosition, myPath[myPathTarget])) {
				myPathTarget++;
			}
		}
	}
	else {
		myPath = CAStar::GetInstance()->GetPath(aPosition, myPositions[myTarget], myNavMesh);
	}

	Vector3 target;
	target = myPositions[myTarget];

	if (!myPath.empty()) {
		if (myPathTarget < myPath.size()) {
			target = myPath[myPathTarget];
		}
	}

	/*CDebug::GetInstance()->DrawLine(myPositions[myTarget], myPath[0]);
	for (int i = 0; i < myPath.size() - 1; i++) {
		CDebug::GetInstance()->DrawLine(myPath[i], myPath[i + 1]);
	}
	CDebug::GetInstance()->DrawLine(myPath.back(), myPositions[myTarget]);*/
	Vector3 direction = target - aPosition;
	direction.Normalize();
	return std::move(direction);
}

bool CPatrol::CheckIfOverlap(const Vector3& aFirstPosition, const Vector3& aSecondPosition)
{
	float xDifference = fabs(aFirstPosition.x - aSecondPosition.x);
	float zDifference = fabs(aFirstPosition.z - aSecondPosition.z);
	if (xDifference > 0.1f)
		return false;
	if (zDifference > 0.1f)
		return false;
	return true;
}

SNavMesh* CPatrol::SetNavMesh()
{
	return nullptr;
}

CSeek::CSeek() :myTarget(nullptr)
{

}

Vector3 CSeek::Update(const Vector3& aPosition)//aPostion == EnemyRobot Position
{
	if (!myTarget)
		return Vector3();

	Vector3 direction = myTarget->Position() - aPosition;
	direction.Normalize();

	return std::move(direction);
}

void CSeek::SetTarget(CTransformComponent* aTarget) {
	myTarget = aTarget;
}

CAttack::CAttack(CEnemyComponent* aUser) : myDamage(1.0f), myTarget(nullptr), myAttackCooldown(1.f), myAttackTimer(0.f), myUser(aUser), myAnimationOffset(0.45f), myIsAttacking(false) {
}

Vector3 CAttack::Update(const Vector3& aPosition)
{
	if (!myTarget) {
		return Vector3();
	}
	Vector3 direction = myTarget->WorldPosition() - aPosition;

	//byt ut attacktimer och attackcooldown till animationtimer - Alexander Matth�i 2021-05-07
	myAttackTimer += CTimer::Dt();
	if ((myAttackTimer >= myAttackCooldown) && !myIsAttacking)
	{
		myIsAttacking = true;
		CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyAttack, myUser });
	}

	if (myAttackTimer >= (myAttackCooldown + myAnimationOffset))
	{
		Vector3 origin = aPosition;
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 10.0f, CPhysXWrapper::ELayerMask::PLAYER);
		int hits = hit.getNbAnyHits();

		if (hits > 0)
		{
			float damage = 5.0f;
			CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage, &damage });
		}
		myAttackTimer = 0.f;
		myIsAttacking = false;
	}

	return std::move(direction);
}

void CAttack::SetTarget(CTransformComponent* aTarget)
{
	myTarget = aTarget;
}