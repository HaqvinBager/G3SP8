#pragma once
#include "Behaviour.h"

class CGameObject;
class CModel;

namespace Binary {
	struct SInstancedModel;
}

class CInstancedModelComponent : public CBehaviour
{
public:
	CInstancedModelComponent(CGameObject& aParent, const Binary::SInstancedModel& aData);
	//CInstancedModelComponent(CGameObject& aParent, const std::string& aModelPath, const int aMaterialID, const std::vector<DirectX::SimpleMath::Matrix>& aInstancedTransforms, bool aRenderWithAlpha = false);
	~CInstancedModelComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	CModel* GetModel() const { return myModel; }
	const std::vector<DirectX::SimpleMath::Matrix>& GetInstancedTransforms() const { return myIntancedTransforms; }

	const bool RenderWithAlpha() const { return myRenderWithAlpha; }
	const int GetMaterialID() const { return myMaterialID; }
	const int GetMaterialID(const int aMeshIndex) const { return myMaterialIds[aMeshIndex]; }


private:
	bool myRenderWithAlpha;
	std::vector<int> myMaterialIds;
	int myMaterialID;
	//unsigned int myMaterialIndex;
	CModel* myModel;
	std::string myModelPath;
	std::vector<DirectX::SimpleMath::Matrix> myIntancedTransforms;
};

