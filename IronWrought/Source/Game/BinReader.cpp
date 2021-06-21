#include "stdafx.h"

#include "BinReader.h"
#include <fstream>

#include "EngineDefines.h"
#include <EngineException.h>
#include "FolderUtility.h"
#include <cstdio>

using namespace Binary;
CBinReader::CBinReader()
{
}

CBinReader::~CBinReader()
{
}

SVertexPaintCollection CBinReader::LoadVertexPaintCollection(const std::string& aSceneName)
{
	std::string exportPath = ASSETPATH("Assets/Generated/" + aSceneName + "/VertexColors/");
	std::vector<std::string> binPaths = CFolderUtility::GetFileNamesInFolder(exportPath, ".bin");

	if(binPaths.size() == 0)
		return SVertexPaintCollection{ };

	std::ifstream stream;
	stream.open(ASSETPATH("Assets/Generated/" + aSceneName + "/VertexColors/" + binPaths[0]), std::ios::binary);
	if (!stream.is_open())
	{
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File: %s", /*aBinFilePath.c_str()*/ "");
		return SVertexPaintCollection{ };
	}

	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];

	int count = 0;
	ptr += Read(count, ptr);
	SVertexPaintCollection vertexPaintCollection = {};
	vertexPaintCollection.myData.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		SVertexPaintColorData data = { };
		ptr += Read(data.myVertexMeshID, ptr);
		int colorCount = 0;

		ptr += Read(colorCount, ptr);
		data.myColors.resize(colorCount);
		ptr += Read(data.myColors.data()[0], ptr, colorCount);

		int vertexCount = 0;
		ptr += Read(vertexCount, ptr);
		data.myVertexPositions.resize(vertexCount);
		ptr += Read(data.myVertexPositions.data()[0], ptr, vertexCount);

		vertexPaintCollection.myData.push_back(data);
	}

	return vertexPaintCollection;
}

Binary::SLevelData CBinReader::Load(const std::string& aPath)
{
	std::ifstream stream;
	stream.open(aPath, std::ios::binary);
	if (!stream.is_open())
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File");

	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];

	Binary::SLevelData data = {};
	ptr += CopyBin<SInstanceID>()(data.myInstanceIDs, ptr);
	ptr += CopyBin<STransform>()(data.myTransforms, ptr);
	ptr += CopyBin<SModel>()(data.myModels, ptr);
	ptr += CopyBin<SPointLight>()(data.myPointLights, ptr);
	ptr += CopyBin<SSpotLight>()(data.mySpotLights, ptr);
	ptr += CopyBin<SCollider>()(data.myColliders, ptr);
	ptr += CopyBin<SInstancedModel>()(data.myInstancedModels, ptr);
	ptr += CopyBin<SEndEventData>()(data.myEndEventData, ptr);
	ptr += CopyBin<SFlickerData>()(data.myFlickerData, ptr);
	ptr += CopyBin<SAddForceData>()(data.myAddForceData, ptr);

	return std::move(data);
}


void CBinReader::Write(const std::string& aFileNameAndPath, const std::vector<Vector3>& someData)
{
	std::ofstream writer(aFileNameAndPath.c_str(), std::ios_base::out | std::ios_base::binary);
	if (!writer)
	{
		std::cout << "Failed to open a new Binary Writer at FilePath: " << aFileNameAndPath << std::endl;
		return;
	}
	int size = static_cast<int>(someData.size());
	writer.write((char*)&size, sizeof(int));
	writer.write((char*)&someData.data()[0], sizeof(Vector3) * someData.size());
	writer.close();
}

bool CBinReader::Read(const std::string& aFileNameAndPath, std::vector<Vector3>& outData)
{
	std::ifstream stream;
	stream.open(aFileNameAndPath, std::ios::binary);
	if (!stream.is_open())
		return false;
	
	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];
	int count = 0;
	ptr += Read(count, ptr);
	outData.resize(count);
	ptr += Read(outData.data()[0], ptr, count);
	return true;
}