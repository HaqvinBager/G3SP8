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

CPatrol::CPatrol(const std::vector<Vector3>& somePositions, SNavMesh* aNavMesh)
{
	myPositions = somePositions;
	myTarget = 0;
	myPathTarget = 0;
	myNavMesh = aNavMesh;
}

Vector3 CPatrol::Update(const Vector3& aPosition)
{
	if (CheckIfOverlap(aPosition, myPositions[myTarget])) // change patrol points & calculate path
	{
		myLastTarget = myTarget;
		myTarget++;

		if (myTarget >= myPositions.size())
		{
			myTarget = 0;
		}
	}
	myPathTarget = 0;
	myPath.clear();
	//myPath = CAStar::GetInstance()->GetPath(myPositions[myLastTarget], myPositions[myTarget], myNavMesh);
	SetPath(myNavMesh->CalculatePath(aPosition, myPositions[myTarget], myNavMesh), myPositions[myTarget]);
	if (myPath.size() > 0) {
		//CDebug::GetInstance()->DrawLine(myPositions[myTarget], myPath[0], 60);
		/*for (int i = 0; i < myPath.size() - 1; i++) {
			CDebug::GetInstance()->DrawLine(myPath[i], myPath[i + 1], 60);
		}*/
		//CDebug::GetInstance()->DrawLine(myPath.back(), myPositions[myTarget], 60);
	}

	//-------------------Old way----------------------- Kan ta bort utkommenterat senare om allt fungerar som det ska - Alexander Matthäi 2021-05-18
	//if (!myPath.empty()) {//change path point
	//	if (myPathTarget < myPath.size()) {
	//		if (CheckIfOverlap(aPosition, myPath[myPathTarget])) {
	//			myPathTarget++;
	//		}
	//	}
	//}
	//else {//create path to the patrolpoint 
	//	myPath.clear();
	//	myPath = myNavMesh->CalculatePath(aPosition, myPositions[myTarget], myNavMesh);
	//	if (myPath.size() > 0) {
	//		if (myPath.size() > 0) {
	//			//CDebug::GetInstance()->DrawLine(myPositions[myTarget], myPath[0], 60);
	//			for (int i = 0; i < myPath.size() - 1; i++) {
	//				CDebug::GetInstance()->DrawLine(myPath[i], myPath[i + 1], 60);
	//			}
	//			//CDebug::GetInstance()->DrawLine(myPath.back(), myPositions[myTarget], 60);
	//		}
	//	}
	//}

	//Vector3 target;
	//target = myPositions[myTarget]; //sets target to a patrolpoint

	//if (!myPath.empty()) {//sets target to a path point
	//	if (myPathTarget < myPath.size()) {
	//		target = myPath[myPathTarget];
	//	}
	//}
	//
	//Vector3 direction = target - aPosition;
	//direction.Normalize();
	//return std::move(direction);

	//-------------------New way-----------------
	size_t pathSize = myPath.size();
	if (pathSize > 0) {

		Vector3 newPos;
		Vector3 dir;

		float epsilon = 0.05f;

		dir = (myPath[pathSize - 1] - aPosition);
		dir.Normalize();

		if (DirectX::SimpleMath::Vector3::DistanceSquared(aPosition, myPath[pathSize - 1]) < epsilon) {
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
	myPath.emplace_back(aFinalPosition);
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		myPath.emplace_back(aPath[i]);
	}
}

CSeek::CSeek(SNavMesh* aNavMesh): myNavMesh(aNavMesh),myTarget(nullptr){}

Vector3 CSeek::Update(const Vector3& aPosition)//aPostion == EnemyRobot Position
{
	if (!myTarget)
		return Vector3();

	myPathTarget = 0;
	SetPath(myNavMesh->CalculatePath(aPosition, myTarget->Position(), myNavMesh), myTarget->Position());

	//myPath = CAStar::GetInstance()->GetPath(aPosition, myTarget->Position(), myNavMesh);
	
	//if (!myPath.empty()) {//change path point
	//	if (myPathTarget < myPath.size()) {
	//		if (CheckIfOverlap(aPosition, myPath[myPathTarget])) {
	//			myPathTarget++;
	//		}
	//	}
	//}
	//Vector3 target;
	//if (myPathTarget < myPath.size()) {
	//	target = myPath[myPathTarget];
	//}

	//Vector3 direction = target - aPosition;
	//direction.Normalize();

	//return std::move(direction);


	size_t pathSize = myPath.size();
	if (pathSize > 0) { 

		Vector3 newPos;
		Vector3 dir;

		float epsilon = 0.05f;

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
	myPath.emplace_back(aFinalPosition);
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		myPath.emplace_back(aPath[i]);
	}
}

void CSeek::SetTarget(CTransformComponent* aTarget) {
	myTarget = aTarget;
}

CAttack::CAttack(CEnemyComponent* aUser) : myDamage(1.0f), myTarget(nullptr), myAttackCooldown(1.f), myAttackTimer(0.f), myUser(aUser) {}

Vector3 CAttack::Update(const Vector3& aPosition)
{
	if (!myTarget) {
		return Vector3();
	}
	Vector3 direction = myTarget->WorldPosition() - aPosition;

	//byt ut attacktimer och attackcooldown till animationtimer - Alexander Matthäi 2021-05-07
	myAttackTimer += CTimer::Dt();
	if (myAttackTimer >= myAttackCooldown) {
		Vector3 origin = aPosition;
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 10.0f, CPhysXWrapper::ELayerMask::PLAYER);
		int hits = hit.getNbAnyHits();

		/*CLineInstance* myLine2 = new CLineInstance();
		myLine2->Init(CLineFactory::GetInstance()->CreateLine(origin, origin + (direction * 10.f), { 255,0,0,255 }));
		CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);*/

		if (hits > 0) {
			std::cout << "Player Hit " << std::endl;
			float damage = 5.0f;
			CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage, &damage });
			CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyAttack, myUser });
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