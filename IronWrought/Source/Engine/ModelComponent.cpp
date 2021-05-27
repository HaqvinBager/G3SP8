#include "stdafx.h"
#include "ModelComponent.h"
#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"
#include "MaterialHandler.h"
#include "Model.h"
#include <BinReader.h>
#include "JsonReader.h"

CModelComponent::CModelComponent(CGameObject& aParent, const Binary::SModel& aData)
	: CBehavior(aParent)
{
	myModelPath = CJsonReader::Get()->GetAssetPath(aData.assetID);
	myModel = CModelFactory::GetInstance()->GetModel(ASSETPATH(myModelPath));	
	myMaterialIDs = aData.materialIDs;
	for(const auto materialID : aData.materialIDs)
		myModel->AddMaterial(CMainSingleton::MaterialHandler().RequestMaterial(materialID));	
}

//CModelComponent::CModelComponent(CGameObject& aParent, const std::string& aFBXPath) : CBehaviour(aParent) {
//	myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
//	//myModel->SetMaterials(CMainSingleton::MaterialHandler().RequestMaterial())
//	std::cout << "Add - ModelComponent: " << aFBXPath << std::endl;
//	myModelPath = aFBXPath;
//	//myRenderWithAlpha = false;
//	std::vector<std::string> materialNames = myModel->GetModelData().myMaterialNames;
//	for (auto& materialName : materialNames)
//	{
//		if (materialName.substr(materialName.size() - 2, 2) == "AL")
//		{
//			myRenderWithAlpha = true;
//			break;
//		}
//	}
//}

CModelComponent::~CModelComponent()
{
	CModelFactory::GetInstance()->ClearModel(myModelPath);

	for (const auto& vertexPaintMaterialName : myVertexPaintMaterialNames)
	{
		CMainSingleton::MaterialHandler().ReleaseMaterial(vertexPaintMaterialName);
	}
}

void CModelComponent::InitVertexPaint(std::vector<SVertexPaintColorData>::const_iterator& it, const std::vector<std::string>& someMaterials)
{
	SVertexPaintData vertexPaintData = CMainSingleton::MaterialHandler().RequestVertexColorID(it, ASSETPATH(myModelPath), someMaterials);
	myVertexPaintColorID = vertexPaintData.myVertexColorID;
	myVertexPaintMaterialNames = vertexPaintData.myRGBMaterialNames;
}

void CModelComponent::Awake()
{}

void CModelComponent::Start()
{}

void CModelComponent::Update()
{}

void CModelComponent::SetModel(const std::string& aFBXPath)
{
	myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
}

CModel* CModelComponent::GetMyModel() const
{
	return myModel;
}

void CModelComponent::OnEnable()
{
}

void CModelComponent::OnDisable()
{
}

const unsigned int CModelComponent::VertexPaintColorID() const 
{
	return myVertexPaintColorID;
}

const std::vector<std::string>& CModelComponent::VertexPaintMaterialNames() const
{
	return myVertexPaintMaterialNames;
}