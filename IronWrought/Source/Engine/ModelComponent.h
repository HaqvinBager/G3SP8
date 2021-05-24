#pragma once
#include "Behaviour.h"

class CGameObject;
class CModel;
struct SVertexPaintColorData;

namespace Binary {
	struct SModel;
}

enum class LOD {
	LOD_High,
	LOD_Medium,
	LOD_Low
};

/*
std::vector<SMaterial> <--

*/
struct SMaterial {

	const int GetMaterial() const { return myMaterialLods[static_cast<size_t>(myCurrentLod)]; }

private:
	LOD myCurrentLod;
	std::array<int, 3> myMaterialLods;
};

class CModelComponent : public CBehaviour
{
public:
	CModelComponent(CGameObject& aParent, const Binary::SModel& aData);

	//CModelComponent(CGameObject& aParent, const std::string& aFBXPath);
	//CModelComponent(CGameObject& aParent, const std::string& aFBXPath, const int aVertexColorsID);
	~CModelComponent() override;


	void InitVertexPaint(std::vector<SVertexPaintColorData>::const_iterator& it, const std::vector<std::string>& someMaterials);

	void Awake() override;
	void Start() override;
	void Update() override;

	void SetModel(const std::string& aFBXPath);

	CModel* GetMyModel() const;
	const std::string& GetModelPath() const { return myModelPath; }
	const unsigned int GetMaterialIndex() const { return myMaterialIndex; }

	const int GetMaterialID(const unsigned int aMeshIndex) const { return myMaterialIDs[aMeshIndex]; }
	const unsigned int GetMaterialCount() const { return static_cast<unsigned int>(myMaterialIDs.size()); }

	void OnEnable() override;
	void OnDisable() override;

public:
	const unsigned int VertexPaintColorID() const;
	const std::vector<std::string>& VertexPaintMaterialNames() const;
	//const bool RenderWithAlpha() const { return myRenderWithAlpha; }

private:
	CModel* myModel;
	std::string myModelPath;
	std::vector<std::string> myVertexPaintMaterialNames;
	unsigned int myVertexPaintColorID;
	//bool myRenderWithAlpha;
	unsigned int myMaterialIndex;
	std::vector<int> myMaterialIDs;
};