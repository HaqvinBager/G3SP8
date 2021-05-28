#include "stdafx.h"
#include "JsonReader.h"
#include <filesystem>
#include "ModelFactory.h"

namespace fs = std::filesystem;

CJsonReader* CJsonReader::ourInstance = nullptr;
const std::string CJsonReader::ourErrorString = "";

CJsonReader* CJsonReader::Get()
{
	if (ourInstance == nullptr)
		ourInstance = new CJsonReader();
	return ourInstance;
}

rapidjson::Document CJsonReader::LoadDocument(const std::string& json_path)
{
	std::string path;
	path = json_path;
	std::ifstream stream(path);
	rapidjson::IStreamWrapper wrapper(stream);
	rapidjson::Document document;
	document.ParseStream(wrapper);
	return document;
}

bool CJsonReader::IsValid(const rapidjson::Document& aDoc, const std::vector<std::string>& someMembers)
{
	if (aDoc.HasParseError())
		return false;

	for (const auto& member : someMembers)
	{
		if (!aDoc.HasMember(member.c_str()))
			return false;
	}

	return true;
}

bool CJsonReader::HasParseError(const rapidjson::Document& aDoc)
{
	return aDoc.HasParseError();
}

void CJsonReader::InitFromGenerated()
{
	const auto& doc = LoadDocument(ASSETPATH("Assets/Generated/Resource_Assets.json"));
	if (!IsValid(doc, { "models", "vertexColors" }))
		return;

	//std::vector<std::string> meshPaths;
	//meshPaths.reserve(doc.GetObjectW()["models"].GetArray().Size());
	for (const auto& model : doc.GetObjectW()["models"].GetArray())
	{
		myPathsMap[model["id"].GetInt()] = model["path"].GetString();
		//meshPaths.push_back(ASSETPATH(model["path"].GetString()));
	}
	//CModelFactory::GetInstance()->LoadMeshesAsync(meshPaths);


	for (const auto& vertexColor : doc.GetObjectW()["vertexColors"].GetArray())
	{
		myPathsMap[vertexColor["id"].GetInt()] = vertexColor["path"].GetString();
	}

	for (const auto& texture : doc.GetObjectW()["textures"].GetArray())
	{
		myPathsMap[texture["id"].GetInt()] = texture["path"].GetString();
	}

	for (const auto& material : doc.GetObjectW()["materials"].GetArray())
	{
		std::vector<int> textureIds = {};
		for (const auto& textureID : material["textureAssets"].GetArray())
		{
			textureIds.push_back(textureID.GetInt());
		}
		int materialID = material["id"].GetInt();
		myMaterialsMap[materialID] = textureIds;
		std::string materialName = material["name"].GetString();
		myMaterialsNameMap[materialID] = materialName;
		myMaterialsIDMap[materialName] = materialID;
	}
}

const bool CJsonReader::HasAssetPath(const int anAssetID) const
{
	return myPathsMap.find(anAssetID) != myPathsMap.end();
}
const bool CJsonReader::TryGetAssetPath(const int anAssetID, std::string& outPath) const
{
	if (myPathsMap.find(anAssetID) == myPathsMap.end())
	{
		outPath = "";
		return false;
	}
	assert(myPathsMap.find(anAssetID) != myPathsMap.end() && "Could not find AssetPath");
	outPath = myPathsMap.at(anAssetID);
	return outPath.size() > 0;
}
const bool CJsonReader::TryGetMaterialsPath(const int aMaterialID, std::vector<std::string>& outTexturePaths) const
{
	if (myMaterialsMap.find(aMaterialID) == myMaterialsMap.end())
	{
		outTexturePaths = {};
		return false;
	}

	const std::vector<int>& textureIds = myMaterialsMap.at(aMaterialID);
	for (const auto& textureID : textureIds)
	{
		if (myPathsMap.find(textureID) != myPathsMap.end())
		{
			outTexturePaths.push_back(myPathsMap.at(textureID));
		}
	}
	return true;
}
const bool CJsonReader::TryGetMaterialsPath(const int aMaterialID, std::array<std::string, 3>& outTexturePaths) const
{
	if (myMaterialsMap.find(aMaterialID) == myMaterialsMap.end())
	{
		outTexturePaths = {};
		return false;
	}
	const std::vector<int>& textureIds = myMaterialsMap.at(aMaterialID);
	for (const auto& textureID : textureIds)
	{		
		const std::string& texturePath = myPathsMap.at(textureID);
		size_t indexOfType = texturePath.find_last_of('_');
		char textureType = texturePath[indexOfType + static_cast<size_t>(1)];
	
		textureType = static_cast<char>(std::toupper(textureType));
		
		if (textureType == 'A')
		{
			textureType = texturePath[indexOfType - static_cast<size_t>(1)];
		}

		switch (textureType)
		{
		case 'C':
			outTexturePaths[0] = texturePath;
			break;
		case 'M':
			outTexturePaths[1] = texturePath;
			break;
		case 'N':
			outTexturePaths[2] = texturePath;
			break;
		default:		
			//Ping Error here - Invalid Texture Type has been loaded. This should be caught earlier in the pipeline.
			break;
		}
	}
	return true;
}
const bool CJsonReader::TryGetMaterialName(const int aMaterialID, std::string& outMaterialName) const
{
	if (myMaterialsNameMap.find(aMaterialID) == myMaterialsNameMap.end())
	{
		outMaterialName = "";
		return false;
	}
	outMaterialName = myMaterialsNameMap.at(aMaterialID);
	return true;
}

const bool CJsonReader::TryGetMaterialID(const std::string& aMaterialName, int& outMaterialID) const
{
	if (myMaterialsIDMap.find(aMaterialName) == myMaterialsIDMap.end())
	{
		outMaterialID = 0;
		return false;
	}
	outMaterialID = myMaterialsIDMap.at(aMaterialName);
	return true;
}

const std::string& CJsonReader::GetAssetPath(const int anAssetID) const
{
	if (myPathsMap.find(anAssetID) == myPathsMap.end())
	{
		std::cout << __FUNCTION__ << " Asset ID: " << anAssetID << " could not be found in myPathsMap! Might be an issue with prefab exported from Unity." << std::endl;
		return ourErrorString;
	}
	return myPathsMap.at(anAssetID);
}
