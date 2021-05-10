#pragma once
#include "Behaviour.h"

class CGameObject;
class CModel;
struct SVertexPaintColorData;

class CModelComponent : public CBehaviour
{
public:
	CModelComponent(CGameObject& aParent, const std::string& aFBXPath);
	//CModelComponent(CGameObject& aParent, const std::string& aFBXPath, const int aVertexColorsID);
	~CModelComponent() override;


	void InitVertexPaint(std::vector<SVertexPaintColorData>::const_iterator& it, const std::vector<std::string>& someMaterials);

	void Awake() override;
	void Start() override;
	void Update() override;

	void SetModel(const std::string& aFBXPath);

	CModel *GetMyModel() const;
	const std::string& GetModelPath() const { return myModelPath; }

	void OnEnable() override;
	void OnDisable() override;

public:
	const unsigned int VertexPaintColorID() const;
	const std::vector<std::string>& VertexPaintMaterialNames() const;
	const bool RenderWithAlpha() const { return myRenderWithAlpha; }

private:
	CModel* myModel;
	std::string myModelPath;
	std::vector<std::string> myVertexPaintMaterialNames;
	unsigned int myVertexPaintColorID;
	bool myRenderWithAlpha;
};