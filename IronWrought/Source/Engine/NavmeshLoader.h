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


	STriangle* GetTriangleAtPoint(DirectX::SimpleMath::Vector3 aPosition);
	std::vector<Vector3> CalculatePath(Vector3 aStartPosition, DirectX::SimpleMath::Vector3 aDestination, SNavMesh* aNavMesh);
	std::vector<Vector3> ReversePath(std::vector<Vector3> aPath, DirectX::SimpleMath::Vector3 aDestination);
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

