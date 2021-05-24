#pragma once
#include <string>
#include <map>
#include "FBXLoaderCustom.h"
#include <mutex>
#include <future>

class CEngine;
class CModel;

struct ID3D11Device;

class CModelFactory {
	friend class CEngine;
public:
	static CModelFactory* GetInstance();
	bool Init(CDirectXFramework* aFramework);
	void ClearFactory();
	



	CModel* GetModel(const std::string& aFilePath);
	CModel* GetInstancedModel(const std::string& aFilePath, int aNumberOfInstanced);	
	CModel* GetOutlineModelSubset();

	std::vector<DirectX::SimpleMath::Vector3>& GetVertexPositions(const std::string& aFilePath);

	CLoaderMesh*& GetMeshes(const std::string& aFilePath);

	void ClearModel(const std::string& aFilePath, int aNumberOfInstances = 0);

private:
	struct SInstancedModel
	{
		const std::string myModelType;
		const int myCount;
		size_t myModelTypeHashCode;

		SInstancedModel(const std::string aModelType, const int aCount)
			: myModelType(aModelType)
			, myCount(aCount)
		{
			myModelTypeHashCode = std::hash<std::string>()(aModelType);
		}
		bool operator< (const SInstancedModel& lhs) const
		{
			return std::tie(myCount, myModelTypeHashCode) < std::tie(lhs.myCount, lhs.myModelTypeHashCode);
		}
	};

private:
	CModel* LoadModel(const std::string& aFilePath);
	CModel* CreateInstancedModels(const std::string& aFilePath, int aNumberOfInstanced);
	ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, const std::string& aTexturePath);

private:
	CModelFactory();
	~CModelFactory();

private:
	std::map<std::string, CModel*> myModelMap;
	std::map<SInstancedModel, CModel*> myInstancedModelMap;
	std::map<std::string, int> myModelMapReferences;
	std::map<std::string, int> myInstancedModelMapReferences;
	std::map<std::string, std::vector<DirectX::SimpleMath::Vector3>> myFBXVertexMap;
	std::map<std::string, CLoaderMesh*> myMeshesMap;
	//CEngine* myEngine;
	CDirectXFramework* myFramework;
	CModel* myOutlineModelSubset;
	static CModelFactory* ourInstance;


public:
	void LoadMeshesAsync(const std::vector<std::string>& someFilePaths);

private:
	void LoadModelAsync(const std::string& aFilePath);

	std::mutex myMeshMutex;
	std::vector<std::future<void>> myFutures;

};