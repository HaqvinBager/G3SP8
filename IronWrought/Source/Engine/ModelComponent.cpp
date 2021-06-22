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
	if (myModelPath.empty())
	{
		myModelPath = "Assets/IronWrought/Mesh/Error/error_model.fbx";
		std::string msg = "Asset ID: " + std::to_string(aData.assetID);
		msg.append(" has issues with the model, path was not found. Probably an issue with the prefab in Unity.");
		std::cout << __FUNCTION__ << " " << msg << std::endl;
		//ENGINE_BOOL_POPUP(false, msg.c_str())
	}
	myModel = CModelFactory::GetInstance()->GetModel(ASSETPATH(myModelPath));	
	myMaterialIDs = aData.materialIDs;
	for(const auto materialID : aData.materialIDs)
		myModel->AddMaterial(CMainSingleton::MaterialHandler().RequestMaterial(materialID));	
}

CModelComponent::CModelComponent(CGameObject& aParent, const std::string& aFBXPath, const std::vector<int>& someMaterialID) 
	: CBehavior(aParent) 
{
	myModelPath = aFBXPath;
	myModel = CModelFactory::GetInstance()->GetModel(ASSETPATH(myModelPath));

	myMaterialIDs = someMaterialID;
	if (someMaterialID.empty())
	{
		int matID = 0;
		myModel->AddMaterial(CMainSingleton::MaterialHandler().RequestDefualtMaterial(matID));
		myMaterialIDs.push_back(matID);
		return;
	}
	for(const auto materialID : someMaterialID)
		myModel->AddMaterial(CMainSingleton::MaterialHandler().RequestMaterial(materialID));
}

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

const int CModelComponent::GetMaterialID(const unsigned int aMeshIndex) const
{
	assert(myMaterialIDs.empty() == false && "myMaterialIDs is empty!");
	assert(aMeshIndex < myMaterialIDs.size() && "aMeshIndex is incorrect!");
	return myMaterialIDs[aMeshIndex];
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