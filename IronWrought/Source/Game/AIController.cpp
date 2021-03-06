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

#ifdef _DEBUG
#include "ModelComponent.h"
namespace EnemyNavMesh_PathVisualisers
{
	std::vector<CGameObject*> gVisualisers;
	void Reset();
	void Init();
	void AddToScene();
	void SetToPositions(const std::vector<DirectX::SimpleMath::Vector3>& somePositions);
	void PrintPositions(const std::vector<DirectX::SimpleMath::Vector3>& somePositions);
}

#endif

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

#ifdef  _DEBUG
	EnemyNavMesh_PathVisualisers::Init();
#endif //  _DEBUG

}

void CPatrol::Enter(const Vector3& aPosition)
{
	myPath.clear();
	if (myNavMesh == nullptr)
		return;

	if (myPatrolPoints.empty())
		return;

	std::cout << __FUNCTION__ << std::endl;

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
	patrolPointPosition.y = aPosition.y;
	CPatrolPointComponent* patrolPoint;// = FindBestPatrolPoint(aPosition);

	if (CheckIfOverlap(aPosition, patrolPointPosition)) // change patrol points & calculate path
	{
		myPatrolPoints[myTarget]->AddBonusValue(100);
		patrolPoint = FindBestPatrolPoint(aPosition);
		for (int i = 0; i < myPatrolPoints.size(); ++i) {
			if (myPatrolPoints[i] == patrolPoint) {
				myTarget = i;
				patrolPointPosition = myPatrolPoints[myTarget]->GameObject().myTransform->Position();
				patrolPointPosition.y = aPosition.y;
			}
		}
		//CMainSingleton::PostMaster().Send({ EMessageType::EnemyReachedTarget });
		SetPath(myNavMesh->CalculatePath(aPosition, patrolPointPosition, myNavMesh), patrolPointPosition);
	}

	size_t pathSize = myPath.size();
	if (pathSize > 0) {
		//Vector3 newPos;
		Vector3 dir;

#ifdef _DEBUG
		//CDebug::GetInstance()->DrawLine(aPosition, myPath[pathSize - 1], 1.0f);
#endif
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
		aPath[i].y = aFinalPosition.y;
		
		if (aPath[i] != aFinalPosition) {
			myPath.push_back(aPath[i]);
#ifdef _DEBUG
			//CDebug::GetInstance()->DrawLine(aPath[i - 1], aPath[i], 25.0f);
#endif
		}

	}

#ifdef _DEBUG
	EnemyNavMesh_PathVisualisers::SetToPositions(myPath);
#endif
}

CPatrolPointComponent* CPatrol::FindBestPatrolPoint(const Vector3& aPosition)
{
	//const std::vector<CPatrolPointComponent*>& patrolPoints = CEngine::GetInstance()->GetActiveScene().PatrolPoints();
	std::vector<float> intrestValues;
	if (!myPatrolPoints.empty()) {
		for (int i = 0; i < myPatrolPoints.size(); ++i) {
			Vector3 patrolPositions = myPatrolPoints[i]->GameObject().myTransform->Position();
			patrolPositions.y = aPosition.y;
			Vector3 dist = patrolPositions - aPosition;
			float length = dist.LengthSquared() / 10.f;
			myPatrolPoints[i]->AddValue(length);
			intrestValues.push_back(myPatrolPoints[i]->GetIntrestValue());
			//std::cout << "[" << i << "] " << "Length Value: " << patrolPoints[i]->GetIntrestValue() << std::endl;
		}

		float min = *std::min_element(intrestValues.begin(), intrestValues.end());
		//std::cout << "Length Value: " << min << std::endl;

		for (int i = 0; i < myPatrolPoints.size(); ++i) {
			if (myPatrolPoints[i]->GetIntrestValue() == min) {
				return myPatrolPoints[i];
			}
		}
	}
	return nullptr;
}

CSeek::CSeek(SNavMesh* aNavMesh, float aEnemyPositionY) : myNavMesh(aNavMesh), myTarget(nullptr), myPositionY(aEnemyPositionY) {
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
	Vector3 targetPosition = myTarget->Position();
	targetPosition.y = myPositionY;
	SetPath(myNavMesh->CalculatePath(aPosition, targetPosition, myNavMesh), targetPosition);
	aPosition;
}

Vector3 CSeek::Update(const Vector3& aPosition)//aPostion == EnemyRobot Position
{
	if (!myTarget)
		return Vector3();

	myPathTarget = 0;
	float epsilon = 0.5f;
	if (myPath.size() <= 1)
	{
		if (myFoundPlayer == true) 
		{
			Vector3 playerPos = myTarget->Position();
			playerPos.y = aPosition.y;
			SetPath(myNavMesh->CalculatePath(aPosition, playerPos, myNavMesh), playerPos);
			
		}
		else 
		{
			float dist = DirectX::SimpleMath::Vector2::DistanceSquared({ myLastPlayerPosition.x, myLastPlayerPosition.z }, { aPosition.x, aPosition.z });
			myLastPlayerPosition.y = aPosition.y;
			SetPath(myNavMesh->CalculatePath(aPosition, myLastPlayerPosition, myNavMesh), myLastPlayerPosition);
			if (dist < epsilon) 
			{
				CMainSingleton::PostMaster().Send({ EMessageType::EnemyReachedLastPlayerPosition });
			}
		}
	}

	size_t pathSize = myPath.size();
	if (pathSize > 0) {

		Vector3 newPos;
		Vector3 dir;

		/*float*/ epsilon = 0.1f;

		dir = (myPath[pathSize - 1] - aPosition);
		dir.Normalize();

		float dist = DirectX::SimpleMath::Vector3::DistanceSquared(aPosition, myPath[pathSize - 1]);
		if (dist < epsilon) {
			myPath.pop_back();
		}
		return dir;
	}
	return Vector3();
}

void CSeek::ClearPath() {
	myPath.clear();
}

void CSeek::SetPath(std::vector<Vector3> aPath, const Vector3& /*aFinalPosition*/)
{
	if (aPath.empty()) 
	{
		return;
	}

	// Den här raden gjorde vi inte innan
	//Vector3 finalPosInNavmesh = myNavMesh->GetClosestPointInsideTriangle( myNavMesh->ReturnClosestTriangle(aFinalPosition, myNavMesh), aFinalPosition);

	myPath.clear();
	myPath = std::move(aPath);
	for (unsigned int i = 0; i < myPath.size(); ++i) {
		myPath[i].y = myPositionY;
		//if (aPath[i] != aFinalPosition) {
		//	myPath.push_back(aPath[i]);
		//}
	}
#ifdef _DEBUG
	EnemyNavMesh_PathVisualisers::SetToPositions(myPath);
#endif
}

void CSeek::SetTarget(CTransformComponent* aTarget) {
	myTarget = aTarget;
}

void CSeek::Receive(const SMessage& aMsg)
{

	if (aMsg.myMessageType == EMessageType::EnemyFoundPlayer) {
		myFoundPlayer = true;
		myLastPlayerPosition = *static_cast<Vector3*>(aMsg.data);
		myLastPlayerPosition.y = myPositionY;
		std::cout << "Amount Seen Player: " << ++amount << std::endl;
		std::cout << __FUNCTION__ << " ENEMY FOUND PLAYER Player Pos: " << myLastPlayerPosition.x << " " << myLastPlayerPosition.y << " " << myLastPlayerPosition.z  << std::endl;
	}

	if (aMsg.myMessageType == EMessageType::EnemyLostPlayer) {
		myFoundPlayer = false;
		amount = 0;
		//myLastPlayerPosition = *static_cast<Vector3*>(aMsg.data);
		//myLastPlayerPosition.y = myPositionY;
		Vector3 plPos = IRONWROUGHT->GetActiveScene().Player()->myTransform->Position();
		std::cout << __FUNCTION__ << " ENEMY LOST PLAYER  Player Pos: " << plPos.x << " " << plPos.y << " " << plPos.z  << std::endl;
	}
}

CAttack::CAttack() {}

void CAttack::Enter(const Vector3& aPosition)
{
	myPath.clear();
	CMainSingleton::PostMaster().Send({ EMessageType::EnemyAttackedPlayer });
	aPosition;
}

Vector3 CAttack::Update(const Vector3& /*aPosition*/)
{
	return Vector3();
}

void CAttack::ClearPath() {
	myPath.clear();
}

CAlerted::CAlerted(SNavMesh* aNavMesh)
{
	myNavMesh = aNavMesh;
	myAlertedTimer = 0.0f;
	myHasNewTartget = false;
}

void CAlerted::Enter(const Vector3& aPosition)
{
	aPosition;

	myPath.clear();
	myAlertedPosition.y = aPosition.y;
	SetPath(myNavMesh->CalculatePath(aPosition, myAlertedPosition, myNavMesh), myAlertedPosition);
	myHasNewTartget = false;
	myAlertedTimer = myAlertedTimerMax;
}

Vector3 CAlerted::Update(const Vector3& aPosition)
{
	if (myHasNewTartget) {
		myPath.clear();
		myAlertedPosition.y = aPosition.y;
		SetPath(myNavMesh->CalculatePath(aPosition, myAlertedPosition, myNavMesh), myAlertedPosition);
		myAlertedTimer = myAlertedTimerMax;
		myHasNewTartget = false;
	}

	myAlertedTimer -= CTimer::Dt();
	if (myAlertedTimer > (myAlertedTimerMax * myAlertedFactor))
		return Vector3();

	//SetPath(myNavMesh->CalculatePath(aPosition, myAlertedPosition, myNavMesh), myAlertedPosition);

	float dist = DirectX::SimpleMath::Vector3::Distance(myAlertedPosition, aPosition);
	float epsilon = 0.5f;
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
	myHasNewTartget = true;
}

void CAlerted::SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition)
{
	if (aPath.empty()) {
		return;
	}

	myPath.clear();
	myPath.push_back(aFinalPosition);
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		aPath[i].y = aFinalPosition.y;
		if (aPath[i] != aFinalPosition) {
			myPath.push_back(aPath[i]);
		}
		/*if (i > 0) {
			CDebug::GetInstance()->DrawLine(aPath[i - 1], aPath[i], 60.0f);
		}*/
	}

#ifdef _DEBUG
	EnemyNavMesh_PathVisualisers::SetToPositions(myPath);
#endif
}

const float CAlerted::PercentileAlertedTimer() const
{
	if (myAlertedTimer <= 0.0f)
		return 0.0f;

	float percentile = myAlertedTimer / (myAlertedTimerMax  * myAlertedFactor);
	percentile = std::clamp(percentile, 0.0f, 1.0f);
	return percentile;
}

CIdle::CIdle()
	: myTargetPosition(Vector3::Zero)
{
}

void CIdle::Enter(const Vector3& /*aPosition*/)
{
}

Vector3 CIdle::Update(const Vector3& aPosition)
{
	//myTarget is not set so we crash... See fix in EnemyComponent.cpp row 129 - 132 maybe I broke its direction so please check it out - Alexander Matthäi 2021-06-13
	Vector3 targetPosition = myTargetPosition;
	targetPosition.y = aPosition.y;
	Vector3 dir = targetPosition - aPosition;
	dir.Normalize();
	return dir;
}

void CIdle::ClearPath()
{
}

void CIdle::SetTarget(const Vector3& aTargetPosition)
{
	myTargetPosition = aTargetPosition;
}

CDetection::CDetection()
	: myDetectionTimer(myDetectionTimerMax)
	, myTarget(nullptr)
{}

void CDetection::Enter(const Vector3 & /*aPosition*/)
{
	myDetectionTimer = myDetectionTimerMax;
}

Vector3 CDetection::Update(const Vector3 & aPosition)
{
	myDetectionTimer -= CTimer::Dt();

	Vector3 targetPosition = myTarget->Position();
	targetPosition.y = aPosition.y;
	Vector3 dir = targetPosition - aPosition;
	dir.Normalize();
	return dir;
	//return Vector3();
}

void CDetection::ClearPath()
{}

void CDetection::SetTarget(CTransformComponent* aTarget)
{
	myTarget = aTarget;
}

const float CDetection::PercentileOfTimer() const
{
	if (myDetectionTimer <= 0.0f)
		return 0.0f;

	float percentile = myDetectionTimer / (myDetectionTimerMax  * myDetectionFactor);
	percentile = std::clamp(percentile, 0.0f, 1.0f);
	return percentile;
}

#ifdef _DEBUG
namespace EnemyNavMesh_PathVisualisers
{
	void Reset()
	{
		for (auto& go : gVisualisers)
		{
			go->myTransform->Position({ 0xDEAD, 0xDEAD, 0xDead });
			go->Active(false);
		}
	}
	void Init()
	{
		gVisualisers.reserve(100);
		for (auto i = 0; i < 100; ++i)
		{
			gVisualisers.push_back(new CGameObject(987 + i));
			gVisualisers.back()->AddComponent<CModelComponent>(*gVisualisers.back(), "Assets/IronWrought/DebugModels/Sphere.fbx", std::vector<int>());
		}
		AddToScene();
		Reset();
	}

	void AddToScene()
	{
		IRONWROUGHT->GetActiveScene().AddInstances(gVisualisers);
	}

	void SetToPositions(const std::vector<DirectX::SimpleMath::Vector3>& somePositions)
	{
		//std::cout << __FUNCTION__ << " Setting gVisualisers to somePositions, count: [ " << somePositions.size() << " ] " << std::endl;
		Reset();
		for (int i = 0; i < somePositions.size(); ++i)
		{
			if (i >= gVisualisers.size())
				break;

			gVisualisers[i]->myTransform->Position(somePositions[i]);
			gVisualisers[i]->Active(true);
		}
		if (gVisualisers.size() > 0)
		{
			Vector3 lastPositionOffset = { 0.0f, 0.5f, 0.0f };
			gVisualisers[0]->myTransform->Position(gVisualisers[0]->myTransform->Position() + lastPositionOffset);
		}
		//PrintPositions(somePositions);
	}

	void PrintPositions(const std::vector<DirectX::SimpleMath::Vector3>& somePositions)
	{
		std::cout << __FUNCTION__ << " Printing positions. Count: [ " << somePositions.size() << " ] " << std::endl;
		for (int i = 0; i < somePositions.size(); ++i)
		{
			auto& pos = somePositions[i];
			std::cout << "\t L  " << "# " << i << " : | x: " << pos.x << " | y: " << pos.y << " | z: " << pos.z << std::endl;
		}
	}
}
#endif
