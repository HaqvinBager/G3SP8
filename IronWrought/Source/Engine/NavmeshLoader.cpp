#include "stdafx.h"
#include "NavmeshLoader.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <fstream>
#include <string>
#include <iostream>
#include <assimp/scene.h>
#include "AStar.h"

bool CNavmeshLoader::Init()
{
	return false;
}

SNavMesh* CNavmeshLoader::LoadNavmesh(std::string aFilepath)
{
	std::ifstream infile(aFilepath);
	if (!infile.good())
	{
		ENGINE_ERROR_BOOL_MESSAGE(false, "Navmesh file could not be found.");
		return nullptr;
	}

	SNavMesh* navMesh = new SNavMesh();
	const struct aiScene* scene = NULL;

	scene = aiImportFile(aFilepath.c_str(), aiProcess_ValidateDataStructure | aiProcess_JoinIdenticalVertices |/* aiProcess_MakeLeftHanded | aiProcess_FlipUVs | aiProcess_FlipWindingOrder |*/ 0);

	if (!scene) {
		ENGINE_ERROR_BOOL_MESSAGE(false, "Navmesh could not be loaded.");
		return nullptr;
	}

	aiMesh* mesh = scene->mMeshes[0];
	MakeTriangles(mesh, navMesh);

	return navMesh;
}

void CNavmeshLoader::MakeTriangles(aiMesh* aMesh, SNavMesh* aNavMesh)
{
	UINT numberOfFaces = aMesh->mNumFaces;
	std::vector<STriangle*> triangles;
	triangles.reserve(numberOfFaces);

	for (UINT i = 0; i < numberOfFaces; ++i)
	{
		triangles.push_back(new STriangle());
		for (UINT j = 0; j < 3; ++j) {
			triangles[i]->myVertexPositions[j] = {
				aMesh->mVertices[aMesh->mFaces[i].mIndices[j]].x,
				aMesh->mVertices[aMesh->mFaces[i].mIndices[j]].y,
				aMesh->mVertices[aMesh->mFaces[i].mIndices[j]].z
			};
			triangles[i]->myIndices[j] = aMesh->mFaces[i].mIndices[j];
		}
		triangles[i]->myId = i;
		triangles[i]->myCenterPosition = GetCentroid(triangles[i]->myVertexPositions[0], triangles[i]->myVertexPositions[1], triangles[i]->myVertexPositions[2]);
	}

	for (UINT i = 0; i < triangles.size() - 1; ++i) {
		for (UINT j = i + 1; j < triangles.size(); ++j) {
			if (AreNeighbors(triangles[i]->myIndices, triangles[j]->myIndices)) {
				triangles[i]->myNeighbors.push_back(triangles[j]);
				triangles[j]->myNeighbors.push_back(triangles[i]);
			}
		}
	}

	aNavMesh->myTriangles = triangles;
}

Vector3 CNavmeshLoader::GetCentroid(Vector3& aVectorOne, Vector3& aVectorTwo, Vector3& aVectorThree)
{
	return {
		(aVectorOne.x + aVectorTwo.x + aVectorThree.x) / 3.0f,
		(aVectorOne.y + aVectorTwo.y + aVectorThree.y) / 3.0f,
		(aVectorOne.z + aVectorTwo.z + aVectorThree.z) / 3.0f
	};
}

bool CNavmeshLoader::AreNeighbors(UINT* someIndices, UINT* someOtherIndices)
{
	UINT counter = 0;
	for (UINT i = 0; i < 3; ++i)
	{
		for (UINT j = 0; j < 3; ++j)
		{
			if (someIndices[i] == someOtherIndices[j]) {
				++counter;
			}
		}
	}

	return counter >= 2;
}

//Simplified Cross product for 2D (xz-plane)
inline float Sign(Vector3& p1, Vector3& p2, Vector3& p3)
{
	return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.z - p3.z);
}

Vector3 SNavMesh::GetClosestPointInsideTriangle(STriangle* aTriangle, const Vector3& aPosition)
{
	//Case 1
	const float wn = aTriangle->GetWindingNumber(aPosition);
	if (fabs(wn) > 1.f) {
		return aPosition;
	}

	//Case 2
	std::vector<SLineSegment> edgeList = aTriangle->GetEdges();
	for (auto& edge : edgeList) {
		if (edge.IsPointOnSegment(aPosition)) {
			return aPosition;
		}
	}

	//Case 3 - A
	float minDist = FLT_MAX;
	Vector3 closestPoint = aPosition;
	for (auto& edge : edgeList) {
		if (edge.IsProjectionOnSegment(aPosition)) {
			Vector3 pp = edge.GetProjectedPointOnSegment(aPosition);
			float dist = Vector3::Distance(aPosition, pp);
			if (dist < minDist) {
				closestPoint = pp;
				minDist = dist;
			}
		}
	}

	//Case 3 - B
	minDist = FLT_MAX;
	Vector3 closestVertex = aPosition;
	for (auto& vertex : aTriangle->myVertexPositions) {
		float dist = Vector3::Distance(aPosition, vertex);
		if (dist < minDist) {
			closestVertex = vertex;
			minDist = dist;
		}
	}

	if (closestPoint == aPosition) {
		return closestVertex;
	}
	else {
		return closestPoint;
	}
}

STriangle* SNavMesh::GetTriangleAtPoint(Vector3 aPosition)
{
	float d1, d2, d3;
	bool hasNegativeSigns, hasPositiveSigns;

	Vector3 p1;
	Vector3 p2;
	Vector3 p3;

	for (auto& tri : myTriangles)
	{
		p1 = tri->myVertexPositions[0];
		p2 = tri->myVertexPositions[1];
		p3 = tri->myVertexPositions[2];

		d1 = Sign(aPosition, p1, p2);
		d2 = Sign(aPosition, p2, p3);
		//optimization?
		if (d1 * d2 < 0) continue;
		d3 = Sign(aPosition, p3, p1);
		//optimization?
		if (d1 * d3 < 0) continue;

		hasNegativeSigns = (d1 < 0) || (d2 < 0) || (d3 < 0);
		hasPositiveSigns = (d1 > 0) || (d2 > 0) || (d3 > 0);

		// Signs should only be zero or of one sign (plus or minus)
		if (!(hasNegativeSigns && hasPositiveSigns))
			return tri;
	}

	return nullptr;
}

SNavMesh::~SNavMesh()
{
	for (size_t i = 0; i < myTriangles.size(); ++i)
	{
		delete myTriangles[i];
		myTriangles[i] = nullptr;
	}
	myTriangles.clear();
}

std::vector<Vector3> SNavMesh::CalculatePath(Vector3 aStartPosition, DirectX::SimpleMath::Vector3 aDestination, SNavMesh* aNavMesh)
{
	std::vector<DirectX::SimpleMath::Vector3> path;

	auto startPosition = aStartPosition;
	auto startTriangle = aNavMesh->GetTriangleAtPoint(startPosition);
	auto endTriangle = aNavMesh->GetTriangleAtPoint(aDestination);

	if (startTriangle == nullptr)
	{
		ResolveStuck(startTriangle, startPosition, aDestination, aNavMesh);
		if (startTriangle == nullptr)
		{
			// Return closest triangle if ResolveStuck doesn't work
			startTriangle = ReturnClosestTriangle(startPosition, aNavMesh);
		}
	}
	if (endTriangle == nullptr)
	{
		ResolveStuck(endTriangle, aDestination, aDestination, aNavMesh);
		if (endTriangle == nullptr)
		{
			// Return closest triangle if ResolveStuck doesn't work
			endTriangle = ReturnClosestTriangle(aDestination, aNavMesh);
		}
	}	
	Vector3 closestPoint = aDestination;
	if (aNavMesh->GetTriangleAtPoint(aDestination) == nullptr) {
		closestPoint = GetClosestPointInsideTriangle(endTriangle, aDestination);
	}

	path = CAStar::GetInstance()->GetPath(startPosition, closestPoint, aNavMesh, startTriangle, endTriangle);
	//if (aNavMesh->GetTriangleAtPoint(aDestination)) {
		path = ReversePath(path, closestPoint);
	//}
	//else {
	//	path = ReversePath(path);
	//}
	return path;
}

std::vector<Vector3> SNavMesh::ReversePath(std::vector<Vector3> aPath, DirectX::SimpleMath::Vector3 aDestination)
{
	std::vector<Vector3> path;
	path.emplace_back(aDestination);
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		path.emplace_back(aPath[(aPath.size() - 1) - i]);
	}
	return path;
}

std::vector<Vector3> SNavMesh::ReversePath(std::vector<Vector3> aPath)
{
	std::vector<Vector3> path;
	for (unsigned int i = 0; i < aPath.size(); ++i) {
		path.emplace_back(aPath[(aPath.size() - 1) - i]);
	}
	return path;
}

STriangle* SNavMesh::ReturnClosestTriangle(const DirectX::SimpleMath::Vector3& aStartPosition, SNavMesh* aNavMesh)
{
	STriangle* closestTriangle = nullptr;
	float lastDistance = FLT_MAX;
	std::vector<Vector3> closestPoints;
	for (unsigned int i = 0; i < aNavMesh->myTriangles.size(); ++i)
	{
		Vector3 closest = GetClosestPointInsideTriangle(aNavMesh->myTriangles[i], aStartPosition);
		closestPoints.push_back(closest);
	}
	for (unsigned int i = 0; i < closestPoints.size(); ++i) {
		float currentDistance = DirectX::SimpleMath::Vector3::DistanceSquared(aStartPosition, closestPoints[i]);
		if (currentDistance < lastDistance)
		{

			lastDistance = currentDistance;
			closestTriangle = aNavMesh->myTriangles[i];
		}
	}
	return closestTriangle;
}

void SNavMesh::ResolveStuck(STriangle* aStartTriangle, const DirectX::SimpleMath::Vector3& aStartPosition, const DirectX::SimpleMath::Vector3& aFinalPosition, SNavMesh* aNavMesh)
{
	// Try to find triangle in front, behind, to the left and right of player
	float tolerance = 0.1f;
	if (!aStartTriangle)
	{
		DirectX::SimpleMath::Vector3 dir = aFinalPosition;
		dir -= aStartPosition;
		dir.Normalize();
		dir *= tolerance;
		DirectX::SimpleMath::Vector3 newPos = aStartPosition;
		newPos += dir;
		aStartTriangle = aNavMesh->GetTriangleAtPoint(newPos);
	}
	if (!aStartTriangle)
	{
		DirectX::SimpleMath::Vector3 dir = aFinalPosition;
		dir -= aStartPosition;
		dir.Normalize();
		dir *= tolerance;
		DirectX::SimpleMath::Vector3 newPos = aStartPosition;
		newPos -= dir;
		aStartTriangle = aNavMesh->GetTriangleAtPoint(newPos);
	}
	if (!aStartTriangle)
	{
		DirectX::SimpleMath::Vector3 dir = aFinalPosition;
		dir -= aStartPosition;
		DirectX::SimpleMath::Vector3 temp;
		temp.x = -dir.y;
		temp.y = dir.x;
		dir = temp;
		dir.Normalize();
		dir *= tolerance;
		DirectX::SimpleMath::Vector3 newPos = aStartPosition;
		newPos += dir;
		aStartTriangle = aNavMesh->GetTriangleAtPoint(newPos);
	}
	if (!aStartTriangle)
	{
		DirectX::SimpleMath::Vector3 dir = aFinalPosition;
		dir -= aStartPosition;
		DirectX::SimpleMath::Vector3 temp;
		temp.x = -dir.y;
		temp.y = dir.x;
		dir = temp;
		dir.Normalize();
		dir *= tolerance;
		DirectX::SimpleMath::Vector3 newPos = aStartPosition;
		newPos -= dir;
		aStartTriangle = aNavMesh->GetTriangleAtPoint(newPos);
	}
}

bool SNavMesh::CheckIfOverlap(Vector3 aPosition, STriangle* aTriangle)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = Sign(aPosition, aTriangle->myVertexPositions[0], aTriangle->myVertexPositions[1]);
	d2 = Sign(aPosition, aTriangle->myVertexPositions[1], aTriangle->myVertexPositions[2]);
	d3 = Sign(aPosition, aTriangle->myVertexPositions[2], aTriangle->myVertexPositions[0]);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

const float SNavMesh::PathLength(const std::vector<Vector3>& aPath, const Vector3& aCurrentPosition)
{
	float pathLength = 0;
	std::vector<Vector3> newPath;
	newPath.push_back(aCurrentPosition);
	for (int i = 0; i < aPath.size(); ++i)
	{
		newPath.push_back(aPath[i]);
	}
	for (int i = 0; i < newPath.size() - 1; ++i)
	{
		pathLength += Vector3::Distance(newPath[i], newPath[i + 1]);
	}
	return pathLength;
}
