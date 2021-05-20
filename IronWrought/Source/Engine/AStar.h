#include <vector>
#include "NavmeshLoader.h"
#include "SimpleMath.h"

class CAStar {

public:
	static CAStar* GetInstance() { static CAStar astar; return &astar; }
	std::vector<DirectX::SimpleMath::Vector3> GetPath(Vector3 aStartPosision, Vector3 aEndPosision, SNavMesh* aNavMesh, STriangle* aStartTriangle, STriangle* anEndTriangle);
	std::vector<DirectX::SimpleMath::Vector3> GetPath(Vector3 aStartPosision, Vector3 aEndPosision, SNavMesh* aNavMesh/*, STriangle* aStartTriangle, STriangle* anEndTriangle*/);
private:
	std::vector<int> AStar(
		SNavMesh* aNavmesh, STriangle* aStartTriangle, STriangle* anEndTriangle
	);
	// np from pontus
	std::vector<DirectX::SimpleMath::Vector3> GetPortals(std::vector<int> aListOfInts, SNavMesh* aNavmesh);
	std::vector<DirectX::SimpleMath::Vector3> StringPull(Vector3 aStartTriangle, Vector3 anEndTriangle, std::vector<DirectX::SimpleMath::Vector3> somePortals);
	float Triarea2d(const Vector3 a, const Vector3 b, const Vector3 c);
};
