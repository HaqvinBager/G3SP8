#pragma once

#include "SimpleMath.h"
#include <map>
#include <vector>
#include <memory>

#define CLINE_DEFAULT_COLOR {0.87f, 0.87f, 0.87f, 1.0f}
#define CLINE_DEFAULT_COLOR_BOX {2.f, 0.75f, 0.75f, 1.0f}

struct ID3D11Device;
class CDirectXFramework;

class CLine;
class CLineInstance;
class CLineFactory
{
	friend class CEngine;
public:
	enum class EShape
	{
		Custom,
		Circle,
		Sphere,
		Triangle,
		Cone,
		Square,
		Box,
		Grid,
		AxisMarker,
		Line,

		Count
	};

public:
	static CLineFactory* GetInstance();
	bool				 Init(CDirectXFramework* aFramework);

private:
	CLineInstance* RequestShape(const EShape aShape,const DirectX::SimpleMath::Vector4& aColor = CLINE_DEFAULT_COLOR, const std::vector<DirectX::SimpleMath::Vector3>& somePositions = std::vector<DirectX::SimpleMath::Vector3>());
	
public:
	CLine* CreateLine(const DirectX::SimpleMath::Vector3& aFrom, const DirectX::SimpleMath::Vector3& aTo, const DirectX::SimpleMath::Vector4& aColor, const bool aTakeOwnership = false);
	
	// Deprecated, please do not use. Use RequestShape() instead.

	CLine* CreateGrid(const DirectX::SimpleMath::Vector4& aColor);
	CLine* CreateAxisMarker();
	CLine* CreateSquareXZ(const float aWidth, const DirectX::SimpleMath::Vector4& aColor = CLINE_DEFAULT_COLOR);
	CLine* CreateCircleXZ(const float aRadius, const short aResolution = 32,const DirectX::SimpleMath::Vector4& aColor = CLINE_DEFAULT_COLOR);
	CLine* CreateTriangleXZ(const float aLength, const float aWidth, const DirectX::SimpleMath::Vector4& aColor = CLINE_DEFAULT_COLOR);
	CLine* CreatePolygon(const std::vector<DirectX::SimpleMath::Vector3>& somePositions, const DirectX::SimpleMath::Vector4& aColor = CLINE_DEFAULT_COLOR);
	CLine* CreateBox(const float& aHalfSizeX, const float& aHalfSizeY, const float& aHalfSizeZ, const Vector4& aColor = CLINE_DEFAULT_COLOR_BOX);


private:
	CLineFactory();
	~CLineFactory();

	std::map<EShape, CLine*> myShapes;
	std::vector<CLine*> myLines;
	std::vector<std::unique_ptr<CLineInstance>> myLineInstances;

	ID3D11Device* myDevice;
	static CLineFactory* ourInstance;
};

