#include "stdafx.h"
#include "InstancedModelComponent.h"
#include "Model.h"
#include "ModelFactory.h"
#include "JsonReader.h"
#include "MaterialHandler.h"
#include <BinReader.h>

CInstancedModelComponent::CInstancedModelComponent(CGameObject& aParent, const Binary::SInstancedModel& aData)
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
	myModel = CModelFactory::GetInstance()->GetInstancedModel(ASSETPATH(myModelPath), static_cast<int>(aData.transforms.size()));
	for (const auto& materialID : aData.materialIDs)
		myModel->AddMaterial(CMainSingleton::MaterialHandler().RequestMaterial(materialID));

	myMaterialIds = aData.materialIDs;

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
