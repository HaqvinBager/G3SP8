#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
//#include <string>
#include <unordered_map>
#include <vector>

using RapidArray = rapidjson::GenericArray<true, rapidjson::Value>;
using RapidObject = rapidjson::GenericObject<true, rapidjson::Value>;
struct SDirectory {
	std::string myFileName;
	std::vector<std::string> myFiles;
};

class CJsonReader
{
public:
	static CJsonReader* Get();
	rapidjson::Document LoadDocument(const std::string& json_path);
private:
	CJsonReader() {}
	~CJsonReader() {}
	static CJsonReader* ourInstance;

public:
	static bool IsValid(const rapidjson::Document& aDoc, const std::vector<std::string>& someMembers);
	static bool HasParseError(const rapidjson::Document& aDoc);

public:
	void InitFromGenerated();
	const bool HasAssetPath(const int anAssetID) const;
	const bool TryGetAssetPath(const int anAssetID, std::string& outPath) const;

	const bool TryGetMaterialsPath(const int aMaterialID, std::vector<std::string>& outTexturePaths) const;
	const bool TryGetMaterialsPath(const int aMaterialID, std::array<std::string, 3>& outTexturePaths) const;

	const bool TryGetMaterialName(const int aMaterialID, std::string& outMaterialName) const;
	const bool TryGetMaterialID(const std::string& aMaterialName, int& outMaterialID) const;



	const std::string& GetAssetPath(const int anAssetID) const;



private:
	//std::vector<SDirectory> myDirectories;
	std::unordered_map<int, std::string> myPathsMap;
	std::unordered_map<int, std::string> myMaterialsNameMap;
	std::unordered_map<std::string, int> myMaterialsIDMap;

	std::unordered_map<int, std::vector<int>> myMaterialsMap;
	std::unordered_map<int, std::string> myVertexColorsMap;

};