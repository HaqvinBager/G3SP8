#include "stdafx.h"

#include "LineFactory.h"
#include "DirectXFramework.h"
#include "Line.h"
#include "LineInstance.h"

#include <fstream>

namespace SM = DirectX::SimpleMath;

CLineFactory* CLineFactory::ourInstance = nullptr;

CLineFactory* CLineFactory::GetInstance()
{
	return ourInstance;
}

bool CLineFactory::Init(CDirectXFramework* aFramework)
{
	assert(ourInstance != nullptr && "Instance has not been created.");
	if( !ourInstance )
	{
		return false;
	}

	myDevice = aFramework->GetDevice();

	return true;
}

CLineInstance* CLineFactory::RequestShape(const EShape aShape, const DirectX::SimpleMath::Vector4& aColor, const std::vector<DirectX::SimpleMath::Vector3>& somePositions)
{
	CLineInstance* shape = nullptr;
	switch (aShape)
	{
		case EShape::Custom:
		{
			if (somePositions.empty())
				break;

			CLine* line = CreatePolygon(somePositions, aColor);
			myLines.push_back(line);
			myLineInstances.push_back(std::make_unique<CLineInstance>());
			myLineInstances.back()->Init(line, false);
			shape = myLineInstances.back().get();
		}break;

		case EShape::Circle:
		{
			CLine* line = CreateCircleXZ(1.0f, 32, aColor);
			myShapes.emplace(EShape::Circle, line);
			myLineInstances.push_back(std::make_unique<CLineInstance>());
			myLineInstances.back()->Init(line, false);
			shape = myLineInstances.back().get();
		}break;

		case EShape::Sphere:
		{

		}break;

		case EShape::Triangle:
		{

		}break;

		case EShape::Cone:
		{

		}break;

		case EShape::Square:
		{

		}break;

		case EShape::Box:
		{

		}break;

		case EShape::Grid:
		{

		}break;

		case EShape::AxisMarker:
		{

		}break;

		case EShape::Line:
		{

		}break;

		default:break;
	}

	return shape;
}

CLine* CLineFactory::CreateLine(const SM::Vector3& aFrom, const SM::Vector3& aTo, const SM::Vector4& aColor, const bool aTakeOwnership)
{
	HRESULT hResult;
	
	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	} vertices[2] = 
	{
		 {aFrom.x, aFrom.y, aFrom.z, 1,  aColor.x, aColor.y, aColor.z, 1}
		,{aTo.x, aTo.y, aTo.z, 1,		 aColor.x, aColor.y, aColor.z, 1}
	};
	
	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(vertices);
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices;
	
	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	
	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};
	
	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader
	
	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	
	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders
	
	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		 {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout
	
	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= sizeof(vertices) / sizeof(SVertex);
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	if (aTakeOwnership)
	{
		ourInstance->myLines.push_back(line);
	}

	return line;
}
//at 10000: Warning C6262: Function uses '320 852' bytes of stack: exceeds/ analyze:stacksize '16 384'. Consider moving some data to the heap 
CLine* CLineFactory::CreateGrid(const DirectX::SimpleMath::Vector4& aColor)
{
	HRESULT hResult;

	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	}; 
	SVertex xLine[2] = 
	{
		{ 1.0f, 0.0f, -50.0f,	1.f,	aColor.x, aColor.y, aColor.z, aColor.w }
		,{ 1.0f, 0.0f, 50.0f,	1.f,	aColor.x, aColor.y, aColor.z, aColor.w }
	};
	SVertex zLine[2] = 
	{
		{ -50.0f, 0.0f, 1.0f,	1.f,	aColor.x, aColor.y, aColor.z, aColor.w }
		,{ 50.0f, 0.0f, 1.0f,	1.f,	aColor.x, aColor.y, aColor.z, aColor.w }
	};

	SVertex vertices[404];// 404 => Grid that is 100 in size. (2 verts per line * 100 lines per axis * 2 axis + 4 verts for closing)
	{
		int positionMultiplier = -50;
		for (int i = 1; i < 203; i += 2)// X
		{
			vertices[i - 1].myX = xLine[0].myX * positionMultiplier;
			vertices[i - 1].myY = xLine[0].myY;
			vertices[i - 1].myZ = xLine[0].myZ;
			vertices[i - 1].myW = xLine[0].myW;

			vertices[i - 1].myR = xLine[0].myR;
			vertices[i - 1].myG = xLine[0].myG;
			vertices[i - 1].myB = xLine[0].myB;
			vertices[i - 1].myA = xLine[0].myA;
			///////////////////////////////////
			vertices[i].myX = xLine[1].myX * positionMultiplier;
			vertices[i].myY = xLine[1].myY;
			vertices[i].myZ = xLine[1].myZ;
			vertices[i].myW = xLine[1].myW;

			vertices[i].myR = xLine[1].myR;
			vertices[i].myG = xLine[1].myG;
			vertices[i].myB = xLine[1].myB;
			vertices[i].myA = xLine[1].myA;
			++positionMultiplier;
		}
		positionMultiplier = -50;
		for (int i = 203; i < 404; i += 2)// Z
		{
			vertices[i - 1].myX = zLine[0].myX;
			vertices[i - 1].myY = zLine[0].myY;
			vertices[i - 1].myZ = zLine[0].myZ * positionMultiplier;
			vertices[i - 1].myW = zLine[0].myW;

			vertices[i - 1].myR = zLine[0].myR;
			vertices[i - 1].myG = zLine[0].myG;
			vertices[i - 1].myB = zLine[0].myB;
			vertices[i - 1].myA = zLine[0].myA;
			///////////////////////////////////
			vertices[i].myX = zLine[1].myX;
			vertices[i].myY = zLine[1].myY;
			vertices[i].myZ = zLine[1].myZ * positionMultiplier;
			vertices[i].myW = zLine[1].myW;

			vertices[i].myR = zLine[1].myR;
			vertices[i].myG = zLine[1].myG;
			vertices[i].myB = zLine[1].myB;
			vertices[i].myA = zLine[1].myA;
			++positionMultiplier;
		}
	}
	
	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(vertices);
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= sizeof(vertices) / sizeof(SVertex);
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	//Yikes? +8bytes per Line
	// Don't. full explenation in Line.hpp
	//line->SetDevice(myDevice);

	return line;
}

CLine* CLineFactory::CreateAxisMarker()
{
	HRESULT hResult;

	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	} vertices[6] = 
	{
		{0.f, 0.f, 0.f, 1,		1.0f, 0.f, 0.f, 1}
		,{0.5f, 0.f, 0.f, 1,		1.0f, 0.f, 0.f, 1}

		,{0.f, 0.f, 0.f, 1,		 0.f, 1.0f, 0.f, 1}
		,{0.f, 0.5f, 0.f, 1,		 0.f, 1.0f, 0.f, 1}

		,{0.f, 0.f, 0.f, 1,		 0.f, 0.f, 1.0f, 1}
		,{0.f, 0.f, 0.5f, 1,		 0.f, 0.f, 1.0f, 1}
	};

	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(vertices);
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= sizeof(vertices) / sizeof(SVertex);
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	return line;
}

CLine* CLineFactory::CreateSquareXZ( const float aWidth, const DirectX::SimpleMath::Vector4& aColor)
{
	HRESULT hResult;

	float halfWidth = aWidth / 2.0f;
	const DirectX::SimpleMath::Vector3& center = { 0.f,0.f,0.f };
	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	} vertices[8] = 
	{
		 {center.x - halfWidth, center.y, center.z - halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x - halfWidth, center.y, center.z + halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}

		,{center.x + halfWidth, center.y, center.z - halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x + halfWidth, center.y, center.z + halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}

		,{center.x - halfWidth, center.y, center.z - halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x + halfWidth, center.y, center.z - halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}

		,{center.x - halfWidth, center.y, center.z + halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x + halfWidth, center.y, center.z + halfWidth, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
	};

	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(vertices);
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= sizeof(vertices) / sizeof(SVertex);
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	return line;
}

CLine* CLineFactory::CreateCircleXZ(const float aRadius, const short aResolution, const DirectX::SimpleMath::Vector4& aColor)
{
	HRESULT hResult;

	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	};

	float circleRotation		 = 0.0f;
	float maxRotation			 = 6.28f;
	const float rotationStep	 = maxRotation / static_cast<float>(aResolution);

	using namespace DirectX::SimpleMath;
	std::vector<SVertex> vertices;
	vertices.reserve(aResolution * 2);
	for ( circleRotation = 0.0f; circleRotation < maxRotation; circleRotation += rotationStep ) {
		SVertex vert = {};
		vert.myX = aRadius * cos(circleRotation);
		vert.myY = 0.0f;
		vert.myZ = aRadius * sin(circleRotation);
		vert.myW = 1.0f;

		vert.myR = aColor.x;
		vert.myG = aColor.y;
		vert.myB = aColor.z;
		vert.myA = 1.0f;
		vertices.emplace_back(vert);
	}
	size_t originalSize = vertices.size();
	for (size_t i = 0; i < originalSize; ++i)
	{
		vertices.emplace_back(vertices[i]);
	}
	vertices.shrink_to_fit();

	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(SVertex) * static_cast<unsigned int>(vertices.size());
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices.data();

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= static_cast<unsigned int>(vertices.size());
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	return line;
}

CLine* CLineFactory::CreateTriangleXZ(const float aLength, const float aWidth, const DirectX::SimpleMath::Vector4& aColor)
{
	HRESULT hResult;

	float halfWidth = aWidth / 2.0f;
	const DirectX::SimpleMath::Vector3& center = { 0.f,0.f,0.f };
	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	} vertices[6] = 
	{
		 {center.x , center.y, center.z, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x - halfWidth, center.y, center.z + aLength, 1,		aColor.x, aColor.y, aColor.z, 1.0f}

		,{center.x, center.y, center.z, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x + halfWidth, center.y, center.z + aLength, 1,		aColor.x, aColor.y, aColor.z, 1.0f}

		,{center.x - halfWidth, center.y, center.z + aLength, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
		,{center.x + halfWidth, center.y, center.z + aLength, 1,		aColor.x, aColor.y, aColor.z, 1.0f}
	};

	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(vertices);
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= sizeof(vertices) / sizeof(SVertex);
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	return line;
}

CLine* CLineFactory::CreatePolygon(const std::vector<DirectX::SimpleMath::Vector3>& somePositions, const DirectX::SimpleMath::Vector4& aColor)
{

	HRESULT hResult;

	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	};

	std::vector<SVertex> vertices;
	vertices.resize(somePositions.size());
	for (unsigned int i = 0; i < somePositions.size(); ++i) {
		auto& pos = somePositions[i];
		vertices[i].myX = pos.x;
		vertices[i].myY = pos.y;
		vertices[i].myZ = pos.z;
		vertices[i].myW = 1.0f;
		
		vertices[i].myR = aColor.x;
		vertices[i].myG = aColor.y;
		vertices[i].myB = aColor.z;
		vertices[i].myA = aColor.w;
	}

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.ByteWidth = sizeof(SVertex) * static_cast<unsigned int>(vertices.size());
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
	subresourceData.pSysMem = vertices.data();

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if (FAILED(hResult))
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if (FAILED(hResult))
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if (FAILED(hResult))
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if (FAILED(hResult))
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if (!line)
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices = static_cast<unsigned int>(vertices.size());
	lineData.myStride = sizeof(SVertex);
	lineData.myOffset = 0;
	lineData.myVertexBuffer = vertexBuffer;
	lineData.myVertexShader = vertexShader;
	lineData.myPixelShader = pixelShader;
	lineData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout = inputLayout;
	line->Init(lineData);

	return line;
}

CLine* CLineFactory::CreateBox(const float& aHalfSizeX, const float& aHalfSizeY, const float& aHalfSizeZ, const Vector4& aColor)
{
	HRESULT hResult;

	const DirectX::SimpleMath::Vector3& center = { 0.f,0.f,0.f };
	struct SVertex
	{
		float myX, myY, myZ, myW;
		float myR, myG, myB, myA;
	} vertices[26] = 
	{																														//xyz
		 {center.x - aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//-++
		 {center.x - aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//--+

		 {center.x - aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//-++
		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+++

		 {center.x - aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//-++
		 {center.x - aHalfSizeX, center.y + aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//-+-

		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+-+
		 {center.x - aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//--+

		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+-+
		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+--

		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+-+
		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+++

		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+-+
		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+++

		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//++-
		 {center.x - aHalfSizeX, center.y + aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//-+-

		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//++-
		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+--

		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//++-
		 {center.x + aHalfSizeX, center.y + aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//+++


		 {center.x - aHalfSizeX, center.y - aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//---
		 {center.x - aHalfSizeX, center.y - aHalfSizeY, center.z + aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//--+

		 {center.x - aHalfSizeX, center.y - aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//---
		 {center.x - aHalfSizeX, center.y + aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//-+-

		 {center.x - aHalfSizeX, center.y - aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f},//---
		 {center.x + aHalfSizeX, center.y - aHalfSizeY, center.z - aHalfSizeZ, 1,		aColor.x, aColor.y, aColor.z, 1.0f}//+--
	};

	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.ByteWidth =	sizeof(vertices);
	bufferDescription.Usage		=	D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags =	D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = {0};
	subresourceData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer = nullptr;
	hResult = myDevice->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}

	//Shaders
	//	Vertex Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/LineVertexShader.cso",std::ios::binary);
	std::string vsData = {std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>()};

	ID3D11VertexShader* vertexShader = nullptr;
	hResult = myDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	vsFile.close();
	//	!Vertex Shader

	//	Pixel Shader
	std::ifstream psFile;
	psFile.open("Shaders/LinePixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};

	ID3D11PixelShader* pixelShader = nullptr;
	hResult = myDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	psFile.close();
	//	!Pixel Shader
	//!Shaders

	//Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		,{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3D11InputLayout* inputLayout = nullptr;
	hResult = myDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &inputLayout);
	if( FAILED( hResult ) )
	{
		return nullptr;
	}
	//!Input Layout

	CLine* line = new CLine();
	if( !line )
	{
		return nullptr;
	}
	CLine::SLineData lineData;
	lineData.myNumberOfVertices		= sizeof(vertices) / sizeof(SVertex);
	lineData.myStride				= sizeof(SVertex);
	lineData.myOffset				= 0;
	lineData.myVertexBuffer			= vertexBuffer;
	lineData.myVertexShader			= vertexShader;
	lineData.myPixelShader			= pixelShader;
	lineData.myPrimitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	lineData.myInputLayout			= inputLayout;
	line->Init(lineData);

	return line;
}

CLineFactory::CLineFactory()
{
	ourInstance = this;
	myDevice = nullptr;
}
CLineFactory::~CLineFactory()
{
	for (auto& line : ourInstance->myLines)
	{
		delete line;
	}
	ourInstance->myLines.clear();

	for (auto& lineInstance : ourInstance->myLineInstances)
	{
		lineInstance.release();
	}
	ourInstance->myLineInstances.clear();

	auto it = ourInstance->myShapes.begin();
	while (it != ourInstance->myShapes.end())
	{
		delete it->second;
		++it;
	}
	ourInstance->myShapes.clear();

	ourInstance = nullptr;
	myDevice = nullptr;
}
