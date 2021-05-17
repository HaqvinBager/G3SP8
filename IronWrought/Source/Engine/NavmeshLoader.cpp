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

	if (startTriangle == nullptr)
	{
		ResolveStuck(startTriangle, startPosition, aDestination, aNavMesh);
		if (startTriangle == nullptr)
		{
			// Return closest triangle if ResolveStuck doesn't work
			startTriangle = ReturnClosestTriangle(startPosition, aNavMesh);
		}
	}

	path = CAStar::GetInstance()->GetPath(startPosition, aDestination, aNavMesh, startTriangle, aNavMesh->GetTriangleAtPoint(aDestination));
	return path;
}

STriangle* SNavMesh::ReturnClosestTriangle(const DirectX::SimpleMath::Vector3& aStartPosition, SNavMesh* aNavMesh)
{
	STriangle* closestTriangle = nullptr;
	float lastDistance = FLT_MAX;
	for (unsigned int i = 0; i < aNavMesh->myTriangles.size(); ++i)
	{
		float currentDistance = DirectX::SimpleMath::Vector3::DistanceSquared(aStartPosition, aNavMesh->myTriangles[i]->myCenterPosition);
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