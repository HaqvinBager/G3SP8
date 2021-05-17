#pragma once
class CTransformComponent;
class CEnemyComponent;
struct SNavMesh;
class CAIController
{
public:
	virtual ~CAIController() { }
	virtual Vector3 Update(const Vector3& aPosition) = 0; // Aki 28/4 anv�nd g�rna const & p� vector3 s� att man inte kopierar 3 floats i varje update :)
	
};

class CPatrol: public CAIController
{
public:
	
	CPatrol(const std::vector<Vector3>& somePositions, SNavMesh* aNavMesh);
	~CPatrol()override {}
	Vector3 Update(const Vector3& aPosition) override;
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
	Vector3 Update(const Vector3& aPosition) override;
	void SetTarget(CTransformComponent* aTarget);
private:
	int myPathTarget;
	int myLastTarget;
	CTransformComponent* myTarget;
	SNavMesh* myNavMesh;
	std::vector<Vector3> myPath;
};

class CAttack : public CAIController
{
public:
	CAttack(CEnemyComponent* aUser);
	~CAttack() override { myTarget = nullptr; }
	Vector3 Update(const Vector3& aPosition) override;
	void SetTarget(CTransformComponent* aTarget);

private:
	float myDamage;
	float myAttackCooldown;
	float myAttackTimer;
	CTransformComponent* myTarget;
	CEnemyComponent* myUser;
};
