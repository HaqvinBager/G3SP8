#include "stdafx.h"
#include "InstancedModelComponent.h"
#include "Model.h"
#include "ModelFactory.h"
#include "JsonReader.h"
#include "MaterialHandler.h"
#include <BinReader.h>

CInstancedModelComponent::CInstancedModelComponent(CGameObject& aParent, const Binary::SInstancedModel& aData)
	: CBehaviour(aParent)
{
	myModelPath = CJsonReader::Get()->GetAssetPath(aData.assetID);
	myModel = CModelFactory::GetInstance()->GetInstancedModel(ASSETPATH(myModelPath), static_cast<int>(aData.transforms.size()));
	for (const auto& materialID : aData.materialIDs)
		myModel->AddMaterial(CMainSingleton::MaterialHandler().RequestMaterial(materialID));

	//const std::vector<std::string>& materialNames = myModel->GetModelInstanceData().myMaterialNames;
	//for (const auto& materialName : materialNames)
	//{
	//	if (materialName.size() > 6)
	//	{
	//		if (materialName.substr(materialName.size() - 6, 2) == "AL")
	//		{
	//			myRenderWithAlpha = true;
	//			break;
	//		}
	//	}
	//}

	myMaterialIds = aData.materialIDs;
	//myMaterialID = aData.materialIDs[0];

	std::vector<Matrix> transforms = {};
	myIntancedTransforms.resize(aData.transforms.size());
	for (unsigned int i = 0; i < aData.transforms.size(); ++i)
	{
		CGameObject temp(0);
		CTransformComponent transform(temp);
		transform.Scale(aData.transforms[i].scale);
		transform.Position(aData.transforms[i].pos);
		transform.Rotation(aData.transforms[i].rot);
		myIntancedTransforms[i] = transform.GetLocalMatrix();
	}
}

//CInstancedModelComponent::CInstancedModelComponent(CGameObject& aParent, const std::string& aModelPath, const int aMaterialID, const std::vector<DirectX::SimpleMath::Matrix>& aInstancedTransforms, bool /*aRenderWithAlpha*/)
//	: CBehaviour(aParent)
//	, myModel(CModelFactory::GetInstance()->GetInstancedModel(aModelPath, static_cast<int> (aInstancedTransforms.size())))
//	, myIntancedTransforms(aInstancedTransforms)
//	, myModelPath(aModelPath)
//	, myMaterialID(aMaterialID)
//{
//	myRenderWithAlpha = false;
//	const std::vector<std::string>& materialNames = myModel->GetModelInstanceData().myMaterialNames;
//	for (const auto& materialName : materialNames)
//	{
//		if (materialName.size() > 6)
//		{
//			if (materialName.substr(materialName.size() - 6, 2) == "AL")
//			{
//				myRenderWithAlpha = true;
//				break;
//			}
//		}
//	}
//}

CInstancedModelComponent::~CInstancedModelComponent()
{
	//SAFE_DELETE(myModel);
	CModelFactory::GetInstance()->ClearModel(myModelPath, static_cast<int>(myIntancedTransforms.size()));
}

void CInstancedModelComponent::Awake()
{
}

void CInstancedModelComponent::Start()
{
}

void CInstancedModelComponent::Update()
{
}

void CInstancedModelComponent::OnEnable()
{
}

void CInstancedModelComponent::OnDisable()
{
}
