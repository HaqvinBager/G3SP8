#pragma once
#include <SimpleMath.h>

class CModel;
struct aiMesh;
struct STriangle;

enum class EStatus {
	UNVISITED,
	OPEN,
	CLOSED
};

struct SLineSegment {
	Vector3 myA;
	Vector3 myB;
	const bool IsPointOnSegment(const Vector3& aPoint) {
		float distAP = Vector3::Distance(myA, aPoint);
		float distBP = Vector3::Distance(myB, aPoint);
		float distAB = Vector3::Distance(myA, myB);
		return distAB == (distAP + distBP);
	}

	const bool IsProjectionOnSegment(const Vector3& aPoint) {
		Vector3 AB = myB - myA;
		Vector3 BP = myB - aPoint;
		float dot = AB.Dot(BP);
		//dot = fabs(dot);
		float max = AB.x * AB.x + AB.y * AB.y + AB.z * AB.z;
		return dot >= 0 && dot <= max;
	}

	Vector3 GetProjectedPointOnSegment(const Vector3& aPoint) {
		Vector3 AB = myB - myA;
		Vector3 PA = aPoint - myA;
		float dot = AB.Dot(PA);
		float lengthAB = AB.Length();
		float lengthPA = PA.Length();
		float cos = dot / (lengthAB * lengthPA);
		float projectedline = cos * lengthPA;
		return myA + (projectedline * AB) / lengthAB;
	}
};

struct STriangle {
	int myId;
	DirectX::SimpleMath::Vector3 myCenterPosition;
	DirectX::SimpleMath::Vector3 myVertexPositions[3];
	UINT myIndices[3];

	float myG = 0;
	float myF = 0;
	int myIndex = -1;
	EStatus myStatus = EStatus::UNVISITED;

	STriangle* myPredecessor = nullptr;
	std::vector<STriangle*> myNeighbors;

	float Round(const float& aNumber) {
		return roundf((aNumber + FLT_EPSILON) * 100.f) / 100.f;
	}

	float GetAngle(const Vector3& A, const Vector3& B, const Vector3& C) {
		Vector3 v0 = { A - B };
		Vector3 v1 = { C - B };
		float angle = atan2f(v1.x, v1.z) - atan2f(v0.x , v0.z);
		float ourPI = 3.14159265f;
		return Round((fabs(angle) > ourPI ? angle < 0 ? angle + (2 * ourPI) : angle - (2 * ourPI) : angle));
	}

	std::vector<SLineSegment> GetEdges() {
		std::vector<SLineSegment> lineSegemts(3);
		lineSegemts[0].myA = myVertexPositions[0];
		lineSegemts[0].myB = myVertexPositions[1];
		lineSegemts[1].myA = myVertexPositions[0];
		lineSegemts[1].myB = myVertexPositions[2];
		lineSegemts[2].myA = myVertexPositions[1];
		lineSegemts[2].myB = myVertexPositions[2];
		return std::move(lineSegemts);
	}

	float GetWindingNumber(const Vector3& aPosition) {
		float result = 0.0f;
		for (auto i = 0; i < 3; ++i) {
			Vector3 vertex = myVertexPositions[i];
			Vector3 nextVertex = i == 2 ? myVertexPositions[0] : myVertexPositions[i + 1];
			float angle = GetAngle(vertex, aPosition, nextVertex);
			result += angle;
		}
		return floorf(result);
	}

	bool operator<(STriangle& other) {
		return this->myG < other.myG;
	}

	bool operator==(STriangle& other) {
		return !(this->myG < other.myG) && !(other.myG < this->myG);
	}

	friend bool operator==(const STriangle& lhs, const STriangle& rhs) {
		return !(lhs.myG < rhs.myG) && !(rhs.myG < lhs.myG);
	}
};

struct SNavMesh {
	std::vector<STriangle*> myTriangles;

	Vector3 GetClosestPointInsideTriangle(STriangle* aTriangle, const Vector3& aPosition);
	STriangle* GetTriangleAtPoint(DirectX::SimpleMath::Vector3 aPosition);
	std::vector<Vector3> CalculatePath(Vector3 aStartPosition, DirectX::SimpleMath::Vector3 aDestination, SNavMesh* aNavMesh);
	std::vector<Vector3> ReversePath(std::vector<Vector3> aPath, DirectX::SimpleMath::Vector3 aDestination);
	std::vector<Vector3> ReversePath(std::vector<Vector3> aPath);
	STriangle* ReturnClosestTriangle(const DirectX::SimpleMath::Vector3& aStartPosition, SNavMesh* aNavMesh);
	void ResolveStuck(STriangle* aStartTriangle, const DirectX::SimpleMath::Vector3& aStartPosition, const DirectX::SimpleMath::Vector3& aFinalPosition, SNavMesh* aNavMesh);
	bool CheckIfOverlap(Vector3 aPosition, STriangle* aTriangle);
	const float PathLength(const std::vector<Vector3>& aPath, const Vector3& aCurrentPosition);

public:
	~SNavMesh();
};

class CNavmeshLoader
{
public:
	bool Init();

	SNavMesh* LoadNavmesh(std::string aFilepath);

private:
	void MakeTriangles(aiMesh* aMesh, SNavMesh* aNavMesh);
	DirectX::SimpleMath::Vector3 GetCentroid(DirectX::SimpleMath::Vector3& aVectorOne, DirectX::SimpleMath::Vector3& aVectorTwo, DirectX::SimpleMath::Vector3& aVectorThree);
	bool AreNeighbors(UINT* someIndices, UINT* someOtherIndices);
};

