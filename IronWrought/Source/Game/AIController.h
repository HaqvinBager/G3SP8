#pragma once
class CTransformComponent;
class CEnemyComponent;
class CPatrolPointComponent;
struct SNavMesh;
class CAIController
{
public:
	virtual ~CAIController() { }
	virtual Vector3 Update(const Vector3& aPosition, CPatrolPointComponent* aPatrolPoint) = 0; // Aki 28/4 använd gärna const & på vector3 så att man inte kopierar 3 floats i varje update :)
	virtual void ClearPath() = 0;
};

class CPatrol: public CAIController
{
public:
	
	CPatrol(const std::vector<Vector3>& somePositions, SNavMesh* aNavMesh);
	~CPatrol()override {}
	Vector3 Update(const Vector3& aPosition, CPatrolPointComponent* aPatrolPoint = nullptr) override;
	void ClearPath() override;
	void SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition);
	//bool CheckIfOverlap(const Vector3& aFirstPosition, const Vector3& aSecondPosition);
	
private:
	std::vector<Vector3> myPositions;
	int myTarget;// Suggestion: rename to myTargetIndex. Reasoning: myTarget suggests the Vec3 position. Adding Index clarifies that it is used to identify the target position. 
	int myPathTarget;
	int myLastTarget;
	SNavMesh* myNavMesh;
	std::vector<Vector3> myPath;
};

class CSeek: public CAIController 
{
public:
	CSeek(SNavMesh* aNavMesh);
	~CSeek()override { myTarget = nullptr; }
	Vector3 Update(const Vector3& aPosition, CPatrolPointComponent* aPatrolPoint = nullptr) override;
	void ClearPath() override;
	void SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition);
	void SetTarget(CTransformComponent* aTarget);
private:
	int myPathTarget;
	CTransformComponent* myTarget;
	SNavMesh* myNavMesh;
	std::vector<Vector3> myPath;
};

class CAttack : public CAIController
{
public:
	CAttack(CEnemyComponent* aUser, Vector3 aResetPosition);
	~CAttack() override { myTarget = nullptr; }
	Vector3 Update(const Vector3& aPosition, CPatrolPointComponent* aPatrolPoint = nullptr) override;
	void ClearPath() override;
	void SetTarget(CTransformComponent* aTarget);

private:
	float myDamage;
	float myAttackCooldown;
	float myAttackTimer;
	CTransformComponent* myTarget;
	CEnemyComponent* myUser;
	std::vector<Vector3> myPath;
	Vector3 myResetPosition;
};
