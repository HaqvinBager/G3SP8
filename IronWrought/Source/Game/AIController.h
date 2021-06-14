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
	virtual Vector3 Update(const Vector3& aPosition) = 0; // Aki 28/4 använd gärna const & på vector3 så att man inte kopierar 3 floats i varje update :)
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
	int amount = 0;
};

class CAttack : public CAIController
{
public:
	CAttack();
	~CAttack() override { }
	void Enter(const Vector3& aPosition)override;
	Vector3 Update(const Vector3& aPosition) override;
	void ClearPath() override;

private:
	std::vector<Vector3> myPath;
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

	const float PercentileAlertedTimer() const;
private:
	Vector3 myAlertedPosition;
	std::vector<Vector3> myPath;
	Vector3 myResetPosition;
	SNavMesh* myNavMesh;
	float myAlertedTimer;
	bool myHasNewTartget;
	const float myAlertedTimerMax = 1.733f;// either 2.167 or 1.733
	const float myAlertedFactor = 0.4f;// Roughly half of the alerted-animation time? (It is roughly 2.0s @ 2021 06 09)
};

class CIdle : public CAIController
{
public:
	CIdle();
	~CIdle() override {}
	void Enter(const Vector3& aPosition) override;
	Vector3 Update(const Vector3& aPosition) override;
	void ClearPath() override;
	void SetTarget(CTransformComponent* aTarget);
private:
	CTransformComponent* myTarget;
};

class CDetection : public CAIController {
public:
	CDetection();
	~CDetection() override {}
	void Enter(const Vector3& aPosition) override;
	Vector3 Update(const Vector3& aPosition) override;
	void ClearPath() override;

	const float PercentileOfTimer() const;
private:
	float myDetectionTimer;
	const float myDetectionTimerMax = 1.733f;// either 2.167 or 1.733
	const float myDetectionFactor = 0.95f;// Roughly half of the alerted-animation time? (It is roughly 2.0s @ 2021 06 09)
};