#include "stdafx.h"
#include "Model.h"
#include "MaterialHandler.h"

CModel::CModel() 
	: myModelData()
	, myModelInstanceData()
	, myInstanceCount(0)
	, myHasBones(false)
{
}

CModel::~CModel() 
{
	for (unsigned int i = 0; i < myModelData.myMaterialNames.size(); ++i)
	{
		CMainSingleton::MaterialHandler().ReleaseMaterial(myModelData.myMaterialNames[i]);
		myModelData.myMaterials[i][0] = nullptr;
		myModelData.myMaterials[i][1] = nullptr;
		myModelData.myMaterials[i][2] = nullptr;
	}

	for (unsigned int i = 0; i < myModelInstanceData.myMaterialNames.size(); ++i)
	{
		CMainSingleton::MaterialHandler().ReleaseMaterial(myModelInstanceData.myMaterialNames[i]);
		myModelInstanceData.myMaterials[i][0] = nullptr;
		myModelInstanceData.myMaterials[i][1] = nullptr;
		myModelInstanceData.myMaterials[i][2] = nullptr;
	}
}

void CModel::Init(SModelData data) {
	myModelData = std::move(data);
}

void CModel::Init(SModelInstanceData data) {
	myModelInstanceData = std::move(data);
}

const CModel::SModelData& CModel::GetModelData() const {
	return myModelData;
}

void CModel::SetMaterials(const std::vector<std::array<ID3D11ShaderResourceView*, 3>>& someMaterials)
{
	myModelData.myMaterials = someMaterials;
}

unsigned int CModel::AddMaterial(const std::array<ID3D11ShaderResourceView*, 3>& aMaterial)
{
 	myModelData.myMaterials.push_back(aMaterial);
	return static_cast<unsigned int>(myModelData.myMaterials.size() - 1);
}

void CModel::AddMaterial(const int aMaterialID)
{
	material mat = CMainSingleton::MaterialHandler().RequestMaterial(aMaterialID);
	


}

void CModel::InstanceCount(int aCount)
{
	myInstanceCount = aCount;
	//myTransforms.reserve(aCount);
}

const bool CModel::ShouldRenderWithAlpha() const
{
	return false;
}

const unsigned int CModel::DetailNormalCount() const
{
	return 0;
}

const unsigned int CModel::UseTrimsheetNumber() const
{
	return false;
}

void CModel::EvaluateModelSuffix(const std::string& aModelSuffix)
{
	size_t startIndex = aModelSuffix.find_first_of('-');
	size_t endIndex = aModelSuffix.find_last_of('.');

	if (startIndex == aModelSuffix.npos)
		return;

	unsigned short modelSuffix = 0;

	std::string subString = aModelSuffix.substr(startIndex, endIndex - startIndex);

	if (subString.find("ap"))
	{
		modelSuffix |= (1 << 0);
	}

	if (subString.find("dn"))
	{
		modelSuffix |= (1 << 4);
	}
}

//bool CModel::AddInstancedTransform(DirectX::SimpleMath::Matrix aTransform)
//{
//	if (myTransforms.size() < myInstanceCount) {
//		myTransforms.emplace_back(aTransform);
//	} else {
//		return false;
//	}
//	return true;
//}
