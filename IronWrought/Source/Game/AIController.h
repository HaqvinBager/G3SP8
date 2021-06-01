#pragma once
class CTransformComponent;
class CEnemyComponent;
class CPatrolPointComponent;
struct SNavMesh;

class CAIController
{
public:
	virtual ~CAIController() { }
	virtual void Enter(const Vector3& aPosition) = 0;
	virtual Vector3 Update(const Vector3& aPosition) = 0; // Aki 28/4 anv�nd g�rna const & p� vector3 s� att man inte kopierar 3 floats i varje update :)
	virtual void ClearPath() = 0;
};

class CPatrol: public CAIController
{
public:
	
	CPatrol(const std::vector<CPatrolPointComponent*>& somePatrolPoints, SNavMesh* aNavMesh);
	~CPatrol()override {}
	void Enter(const Vector3& aPosition)override;
	Vector3 Update(const Vector3& aPosition) override;
	void ClearPath() override;
	void SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition);
	CPatrolPointComponent* FindBestPatrolPoint(const Vector3& aPosition);
	//bool CheckIfOverlap(const Vector3& aFirstPosition, const Vector3& aSecondPosition);
	
private:
	//std::vector<CPatrolPointComponent> myPositions;
	int myTarget;// Suggestion: rename to myTargetIndex. Reasoning: myTarget suggests the Vec3 position. Adding Index clarifies that it is used to identify the target position. 
	int myPathTarget;
	int myLastTarget;
	SNavMesh* myNavMesh;
	std::vector<Vector3> myPath;
	std::vector<CPatrolPointComponent*> myPatrolPoints;
};

class CSeek: public CAIController, public IObserver
{
public:
	CSeek(SNavMesh* aNavMesh);
	~CSeek()override; 
	void Enter(const Vector3& aPosition)override;
	Vector3 Update(const Vector3& aPosition) override;
	void ClearPath() override;
	void SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition);
	void SetTarget(CTransformComponent* aTarget);
	void Receive(const SMessage& aMsg) override;
private:
	int myPathTarget;
	CTransformComponent* myTarget;
	SNavMesh* myNavMesh;
	std::vector<Vector3> myPath;
	bool myFoundPlayer;
	Vector3 myLastPlayerPosition;
};

class CAttack : public CAIController
{
public:
	CAttack(CEnemyComponent* aUser, Vector3 aResetPosition);
	~CAttack() override { myTarget = nullptr; }
	void Enter(const Vector3& aPosition)override;
	Vector3 Update(const Vector3& aPosition) override;
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

class CAlerted : public CAIController {
public:
	CAlerted(SNavMesh* aNavMesh);
	~CAlerted() override {}
	void Enter(const Vector3& aPosition)override;
	Vector3 Update(const Vector3& aPosition) override;
	void ClearPath() override;
	void SetAlertedPosition(const Vector3& aAlertedPosition);
	void SetPath(std::vector<Vector3> aPath, Vector3 aFinalPosition);
private:
	Vector3 myAlertedPosition;
	std::vector<Vector3> myPath;
	Vector3 myResetPosition;
	SNavMesh* myNavMesh;
};