#include "PhysXWrapper.h"
#include "ContactReportCallback.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"
#include "RigidDynamicBody.h"
#include <iostream>
#include "LineFactory.h"
#include "LineInstance.h"
#include "ModelComponent.h"
#include "InstancedModelComponent.h"
#include "ModelFactory.h"
#include "Model.h"
#include "CharacterController.h"
#include <PlayerControllerComponent.h>
#include "RigidBodyComponent.h"
#include "PlayerReportCallback.h"
#include "ConvexMeshColliderComponent.h"
#include "EnemyReportCallback.h"
#include "ContactFilterCallback.h"

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	/*pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eDETECT_CCD_CONTACT
		| PxPairFlag::eTRIGGER_DEFAULT;

	return PxFilterFlag::eDEFAULT;*/
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eSOLVE_CONTACT
		| PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eDETECT_CCD_CONTACT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	//if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	//	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	PxU32 keep = (filterData0.word0 & filterData1.word0)
		| (filterData0.word1 & filterData1.word1)
		| (filterData0.word2 & filterData1.word2)
		| (filterData0.word3 & filterData1.word3);
	if (keep)
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}
	PxFilterFlags flags;
	flags.set(PxFilterFlag::eCALLBACK);
	flags.set(PxFilterFlag::eNOTIFY);
	return flags;
}

CPhysXWrapper::CPhysXWrapper()
{
	myFoundation = nullptr;
	myPhysics = nullptr;
	myDispatcher = nullptr;
	myPXMaterial = nullptr;
	myPhysicsVisualDebugger = nullptr;
	myAllocator = nullptr;
	myContactReportCallback = nullptr;
	myControllerManager = nullptr;
	myPlayerReportCallback = nullptr;
	myContactFilterReportCallback = nullptr;
}

CPhysXWrapper::~CPhysXWrapper()
{
	delete myContactReportCallback;
	//delete myPlayerReportCallback; // inaccesible?

	if (myCooking)
		myCooking->release();
	if (myPXMaterial)
		myPXMaterial->release();
	if (myDispatcher)
		myDispatcher->release();
	//if (myControllerManager)
	//	myControllerManager->release();// Can't release
	if (myPhysics)
		myPhysics->release();
	if (myPhysicsVisualDebugger)
		myPhysicsVisualDebugger->release();

	delete myAllocator;
	myAllocator = nullptr;

	//if(myFoundation)
	//	myFoundation->release();// Can't release

	//I will fix later -- crashes because cant release nullptr //Alexander Matth???i 15/1 - 2021

}

bool CPhysXWrapper::Init()
{
	PxDefaultErrorCallback errorCallback;

	myAllocator = new PxDefaultAllocator();
	myFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myAllocator, errorCallback);

	if (!myFoundation)
	{
		return false;
	}
	myPhysicsVisualDebugger = PxCreatePvd(*myFoundation);
	if (!myPhysicsVisualDebugger)
	{
		return false;
	}
	//Omg ??r det s??h??r vi kopplar v??rt program till PVD Debuggern?! :D
	//ja!!
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
	myPhysicsVisualDebugger->connect(*transport, PxPvdInstrumentationFlag::eALL);
	PxTolerancesScale scale;
	scale.length = 1;
	myCooking = PxCreateCooking(PX_PHYSICS_VERSION, *myFoundation, PxCookingParams(scale));

	myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, PxTolerancesScale(), true, myPhysicsVisualDebugger);
	if (!myPhysics)
	{
		return false;
	}

	// All collisions gets pushed to this class
	myContactReportCallback = new CContactReportCallback();
	myPlayerReportCallback = new CPlayerReportCallback();
	myContactFilterReportCallback = new CContactFilterCallback();
	return true;
}

PxScene* CPhysXWrapper::CreatePXScene()
{
	PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.82f, 0.0f);
	myDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.cpuDispatcher = myDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = myContactReportCallback;
	sceneDesc.filterCallback = myContactFilterReportCallback;
	sceneDesc.flags = PxSceneFlag::eENABLE_CCD;


	PxScene* pXScene = myPhysics->createScene(sceneDesc);
	if (!pXScene)
	{
		return nullptr;
	}

	PxPvdSceneClient* pvdClient = pXScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	myControllerManager = PxCreateControllerManager(*pXScene);
	return pXScene;
}

PxScene* CPhysXWrapper::GetPXScene()
{
	return CEngine::GetInstance()->GetActiveScene().PXScene();
}

bool CPhysXWrapper::TryRayCast(const Vector3& aOrigin, Vector3& aDirection, const float aDistance, PxRaycastBuffer& /*outHit*/)
{
	PxScene* scene = CEngine::GetInstance()->GetActiveScene().PXScene();

	PxVec3 origin;
	origin.x = aOrigin.x;
	origin.y = aOrigin.y;
	origin.z = aOrigin.z;

	PxVec3 unitDir;
	aDirection.Normalize();
	unitDir.x = aDirection.x;
	unitDir.y = aDirection.y;
	unitDir.z = aDirection.z;

	PxReal maxDistance = aDistance;
	PxRaycastBuffer hit;
	bool hasHit = scene->raycast(origin, unitDir, maxDistance, hit);
	return hasHit;
}

PxRaycastBuffer CPhysXWrapper::Raycast(Vector3 aOrigin, Vector3 aDirection, float aDistance, ELayerMask aLayerMask)
{
	PxScene* scene = CEngine::GetInstance()->GetActiveScene().PXScene();
	PxVec3 origin;
	origin.x = aOrigin.x;
	origin.y = aOrigin.y;
	origin.z = aOrigin.z;

	PxVec3 unitDir;
	aDirection.Normalize();
	unitDir.x = aDirection.x;
	unitDir.y = aDirection.y;
	unitDir.z = aDirection.z;

	PxReal maxDistance = aDistance;
	PxRaycastBuffer hit;
	PxQueryFilterData filterData = PxQueryFilterData();
	filterData.data.word0 = static_cast<PxU32>(aLayerMask);
	scene->raycast(origin, unitDir, maxDistance, hit, PxHitFlag::eDEFAULT, filterData);
	return hit;
}

PxRaycastBuffer CPhysXWrapper::Raycast(Vector3 aOrigin, Vector3 aDirection, float aDistance, physx::PxU32 aLayerMask)
{
	PxScene* scene = CEngine::GetInstance()->GetActiveScene().PXScene();
	PxVec3 origin;
	origin.x = aOrigin.x;
	origin.y = aOrigin.y;
	origin.z = aOrigin.z;

	PxVec3 unitDir;
	aDirection.Normalize();
	unitDir.x = aDirection.x;
	unitDir.y = aDirection.y;
	unitDir.z = aDirection.z;

	PxReal maxDistance = aDistance;
	PxRaycastBuffer hit;
	PxQueryFilterData filterData = PxQueryFilterData();
	filterData.data.word0 = aLayerMask;
	scene->raycast(origin, unitDir, maxDistance, hit, PxHitFlag::eDEFAULT, filterData);
	return hit;
}





PxMaterial* CPhysXWrapper::CreateMaterial(materialfriction amaterial)
{
	switch ((materialfriction)amaterial)
	{
	case materialfriction::metal:
		return myPhysics->createMaterial(1.0f, 5.0f, -1.0f);
		break;
	case materialfriction::wood:
		return myPhysics->createMaterial(0.2f, 0.5f, 0.3f);
		break;
	case materialfriction::bounce:
		return myPhysics->createMaterial(0.0f, 0.0f, 1.0f);
		break;
	case materialfriction::basic:
		return myPhysics->createMaterial(0.6f, 0.6f, 0.0f);
		break;
	case materialfriction::none:
		return myPhysics->createMaterial(0.0f, 0.0f, 0.0f);
		break;
	default:

		break;
	}
	return nullptr;
}

PxMaterial* CPhysXWrapper::CreateCustomMaterial(const float& aDynamicFriction, const float& aStaticFriction, const float& aBounciness)
{
	return myPhysics->createMaterial((PxReal)aDynamicFriction, (PxReal)aStaticFriction, (PxReal)aBounciness);
}

void CPhysXWrapper::Simulate()
{
	if (GetPXScene() != nullptr)
	{
		GetPXScene()->simulate(CTimer::FixedDt());
		GetPXScene()->fetchResults(true);
	}
}

CRigidDynamicBody* CPhysXWrapper::CreateDynamicRigidbody(const CTransformComponent& aTransform)
{
	Matrix ironTransform = aTransform.GetLocalMatrix();
	return CreateDynamicRigidbody(ConvertToPxTransform(ironTransform));
}

CRigidDynamicBody* CPhysXWrapper::CreateDynamicRigidbody(const PxTransform& aTransform)
{
	CRigidDynamicBody* dynamicBody = new CRigidDynamicBody(*myPhysics, aTransform);
	GetPXScene()->addActor(dynamicBody->GetBody());
	return dynamicBody;
}

CCharacterController* CPhysXWrapper::CreateCharacterController(const Vector3& aPos, const float& aRadius, const float& aHeight, CTransformComponent* aUserData, physx::PxUserControllerHitReport* aHitReport)
{
	CCharacterController* characterController = new CCharacterController(aPos, aRadius, aHeight, aUserData, aHitReport);
	return characterController;
}

PxControllerManager* CPhysXWrapper::GetControllerManager()
{
	return myControllerManager/*myControllerManagers[GetPXScene()]*/;
}

physx::PxTransform CPhysXWrapper::ConvertToPxTransform(const Matrix& aTransform)
{
	Matrix ironTransform = aTransform;
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
	ironTransform.Decompose(scale, rotation, position);
	PxTransform transform;
	transform.p = { position.x, position.y, position.z };
	transform.q = { rotation.x, rotation.y, rotation.z, rotation.w };
	return transform;
}

//void CPhysXWrapper::DebugLines()
//{
//	const PxRenderBuffer& rb = CEngine::GetInstance()->GetActiveScene().PXScene()->getRenderBuffer();
//	for (PxU32 i = 0; i < rb.getNbLines(); i++)
//	{
//		const PxDebugLine& line = rb.getLines()[i];
//		CLineInstance* myLine = new CLineInstance();
//		myLine->Init(CLineFactory::GetInstance()->CreateLine({ line.pos0.x, line.pos0.y, line.pos0.z }, { line.pos1.x, line.pos1.y, line.pos1.z }, { 0.1f, 255.0f, 0.1f, 1.0f }));
//		CEngine::GetInstance()->GetActiveScene().AddInstance(myLine);
//	}
//}

void CPhysXWrapper::Cooking(const std::vector<CGameObject*>& gameObjectsToCook, CScene* aScene)
{
	//Refactor Start - Axel Savage 2021-05-28
	//Goal Verify SubMeshes Creating Static Collision

	for (const auto& gameObject : gameObjectsToCook)
	{
		CModelComponent* model;
		CConvexMeshColliderComponent* convexCollider;
		CInstancedModelComponent* instancedModel;
		if (gameObject->TryGetComponent(&model) && gameObject->TryGetComponent(&convexCollider))
		{
			convexCollider->SetShape(CookShape(model->GetMyModel(), gameObject->myTransform, convexCollider->GetMaterial()));
		}
		else if (gameObject->TryGetComponent(&instancedModel))
		{
			std::vector<PxRigidStatic*> statics = CookShapes(instancedModel->GetModel(), gameObject->myTransform, CreateMaterial(CPhysXWrapper::materialfriction::basic), instancedModel->GetInstancedTransforms());
			for (const auto& rigidStatic : statics)
				aScene->PXScene()->addActor(*rigidStatic);			
		}
	}
}






PxConvexMeshDesc CPhysXWrapper::GetConvexMeshDesc(const std::vector<PxVec3>& someVerts, const std::vector<unsigned int>& someIndexes, const PxConvexFlags someFlags) const
{
	PxConvexMeshDesc meshDesc;
	meshDesc.points.count = (PxU32)someVerts.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = someVerts.data();
	meshDesc.flags = someFlags;//PxConvexFlag::eCOMPUTE_CONVEX;

	std::vector<unsigned int> indexes = someIndexes;
	meshDesc.indices.count = (PxU32)indexes.size();
	meshDesc.indices.stride = sizeof(PxU32);
	meshDesc.indices.data = indexes.data();
	return meshDesc;
}


PxTriangleMeshDesc CPhysXWrapper::GetTriangleMeshDesc(const std::vector<PxVec3>& someVerts, const std::vector<unsigned int>& someIndices) const
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = (PxU32)someVerts.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = someVerts.data();

	meshDesc.triangles.count = (PxU32)someIndices.size() / (PxU32)3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = someIndices.data();

	return meshDesc;
}



PxShape* CPhysXWrapper::CookShape(const CModel* aModel, const CTransformComponent* aTransform, const physx::PxMaterial* aMaterial) const
{
	const CModel::SModelData& modelData = aModel->GetModelData();
	std::vector<PxVec3> verts = {};
	Convert<Vector3, PxVec3>()(modelData.myMeshFilter.myVertecies, verts);
	const auto meshDesc = GetConvexMeshDesc(verts, modelData.myMeshFilter.myIndexes, PxConvexFlag::eCOMPUTE_CONVEX);

	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	myCooking->cookConvexMesh(meshDesc, writeBuffer, &result);
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	PxConvexMesh* pxMesh = myPhysics->createConvexMesh(readBuffer);
	const auto& scale = aTransform->Scale();
	PxMeshScale meshScale(PxVec3(scale.x, scale.y, scale.z), PxQuat(PxIdentity));
	PxConvexMeshGeometry pMeshGeometry(pxMesh, meshScale);

	PxShape* convexShape = myPhysics->createShape(pMeshGeometry, *aMaterial, true);
	return convexShape;
}

/*
* // Sub-meshes handling: 2021 05 31. Test
	std::vector<PxVec3> verts = {};
	std::vector<const std::vector<Vector3>*> vectorVectors;
	vectorVectors.reserve(modelData.myMeshFilters.size());
	for (auto& meshfilter : modelData.myMeshFilters)
	{
	vectorVectors.push_back(&meshfilter.myVertecies);
	}
	Convert<const std::vector<Vector3>*, PxVec3>()(vectorVectors, verts);
	
	std::vector<unsigned int> indices = {};
	std::vector<const std::vector<unsigned int>*> vectorIndices;
	vectorIndices.reserve(modelData.myMeshFilters.size());
	for (auto& meshfilter : modelData.myMeshFilters)
	{
	vectorIndices.push_back(&meshfilter.myIndexes);
	}
	Convert<const std::vector<unsigned int>*, unsigned int>()(vectorIndices, indices);
	PxTriangleMeshDesc meshDesc = GetTriangleMeshDesc(verts, indices);
*/
std::vector<PxRigidStatic*> CPhysXWrapper::CookShapes(const CModel* aModel, const CTransformComponent* aTransform, const physx::PxMaterial* aMaterial, const std::vector<Matrix>& someTransforms) const
{
	const CModel::SModelInstanceData& modelData = aModel->GetModelInstanceData();
	std::vector<PxRigidStatic*> statics = { };
	statics.reserve(someTransforms.size());
	for (auto& meshFilter : modelData.myMeshFilters)
	{
		std::vector<PxVec3> verts = {};
		Convert<Vector3, PxVec3>()(meshFilter.myVertecies, verts);
		PxTriangleMeshDesc meshDesc = GetTriangleMeshDesc(verts, meshFilter.myIndexes);

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		myCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* pxMesh = myPhysics->createTriangleMesh(readBuffer);

		for (const auto& transform : someTransforms)
		{
			DirectX::SimpleMath::Vector3 translation;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion quat;
			DirectX::SimpleMath::Matrix t = transform;
			t.Decompose(scale, quat, translation);

			PxMeshScale meshScale(PxVec3(scale.x, scale.y, scale.z), PxQuat(PxIdentity));

			PxTriangleMeshGeometry pMeshGeometry(pxMesh, meshScale);

			PxVec3 pos = { translation.x, translation.y, translation.z };
			PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };
			//staticRigidbody->setGlobalPose({ pos, pxQuat });
			PxRigidStatic* staticRigidbody = myPhysics->createRigidStatic({ pos, pxQuat });
			staticRigidbody->userData = (void*)aTransform;
			PxShape* instancedShape = myPhysics->createShape(pMeshGeometry, *aMaterial, true);

			PxFilterData filterData;
			if (aTransform->GameObject().Tag() != "Door" && aTransform->GameObject().Tag() != "Painting") {
				filterData.word0 = static_cast<PxU32>(CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT);
			} 
			instancedShape->setQueryFilterData(filterData);
			staticRigidbody->attachShape(*instancedShape);


			statics.push_back(staticRigidbody);
		}
	}
	return std::move(statics);
}

physx::PxShape* CPhysXWrapper::CookObject(CGameObject& aGameObject)
{
	std::vector<PxVec3> verts(aGameObject.GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies.size());

	for (auto z = 0; z < aGameObject.GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies.size(); ++z)
	{
		Vector3 vec = aGameObject.GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies[z];
		verts[z] = PxVec3(vec.x, vec.y, vec.z);
	}

	PxConvexMeshDesc meshDesc;
	meshDesc.points.count = (PxU32)verts.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts.data();
	meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX /*| PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION*/;

	std::vector<unsigned int> indexes = aGameObject.GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myIndexes;
	meshDesc.indices.count = (PxU32)indexes.size() / (PxU32)3;
	meshDesc.indices.stride = 3 * sizeof(PxU32);
	meshDesc.indices.data = indexes.data();

	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	myCooking->cookConvexMesh(meshDesc, writeBuffer, &result);

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxConvexMesh* pxMesh = myPhysics->createConvexMesh(readBuffer);

	PxConvexMeshGeometry pMeshGeometry(pxMesh);
	PxShape* convexShape = myPhysics->createShape(pMeshGeometry, *CreateMaterial(CPhysXWrapper::materialfriction::basic), true);
	return convexShape;
}


//Axel Savage 2021-05-31 (Completed Refactor of Cooking)
//for (int i = 0; i < gameObjectsToCook.size(); ++i)
//{
//	if (gameObjectsToCook[i]->GetComponent<CModelComponent>() && gameObjectsToCook[i]->GetComponent<CConvexMeshColliderComponent>())
//	{
//		std::vector<PxVec3> verts(gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies.size());

//		for (auto z = 0; z < gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies.size(); ++z)
//		{
//			Vector3 vec = gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies[z];
//			verts[z] = PxVec3(vec.x, vec.y, vec.z);
//		}

//		PxConvexMeshDesc meshDesc;
//		meshDesc.points.count = (PxU32)verts.size();
//		meshDesc.points.stride = sizeof(PxVec3);
//		meshDesc.points.data = verts.data();
//		meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

//		std::vector<unsigned int> indexes = gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myIndexes;
//		meshDesc.indices.count = (PxU32)indexes.size();
//		meshDesc.indices.stride = sizeof(PxU32);
//		meshDesc.indices.data = indexes.data();

//		PxDefaultMemoryOutputStream writeBuffer;
//		PxConvexMeshCookingResult::Enum result;
//		myCooking->cookConvexMesh(meshDesc, writeBuffer, &result);

//		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//		PxConvexMesh* pxMesh = myPhysics->createConvexMesh(readBuffer);

//		PxMeshScale meshScale(
//			PxVec3(gameObjectsToCook[i]->myTransform->Scale().x,
//			gameObjectsToCook[i]->myTransform->Scale().y,
//			gameObjectsToCook[i]->myTransform->Scale().z),
//			PxQuat(PxIdentity));

//		PxConvexMeshGeometry pMeshGeometry(pxMesh, meshScale);

//		PxShape* convexShape = myPhysics->createShape(pMeshGeometry, *gameObjectsToCook[i]->GetComponent<CConvexMeshColliderComponent>()->GetMaterial(), true);
//		gameObjectsToCook[i]->GetComponent<CConvexMeshColliderComponent>()->SetShape(convexShape);


//	}

//	if (gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>() &&
//		!gameObjectsToCook[i]->GetComponent<CPlayerControllerComponent>())
//	{
//		/*

//		*/

//		for (auto z = 0; z < gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>()->GetInstancedTransforms().size(); ++z)
//		{
//			

//			

//			std::vector<PxVec3> verts(gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>()->GetModel()->GetModelInstanceData().myMeshFilter.myVertecies.size());

//			for (auto y = 0; y < gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>()->GetModel()->GetModelInstanceData().myMeshFilter.myVertecies.size(); ++y)
//			{
//				Vector3 vec = gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>()->GetModel()->GetModelInstanceData().myMeshFilter.myVertecies[y];
//				verts[y] = PxVec3(vec.x, vec.y, vec.z);
//			}

//			PxTriangleMeshDesc meshDesc;
//			meshDesc.points.count = (PxU32)verts.size();
//			meshDesc.points.stride = sizeof(PxVec3);
//			meshDesc.points.data = verts.data();

//			std::vector<unsigned int> myInstancedIndexes = gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>()->GetModel()->GetModelInstanceData().myMeshFilter.myIndexes;
//			meshDesc.triangles.count = (PxU32)myInstancedIndexes.size() / (PxU32)3;
//			meshDesc.triangles.stride = 3 * sizeof(PxU32);
//			meshDesc.triangles.data = myInstancedIndexes.data();

//			PxDefaultMemoryOutputStream writeBuffer;
//			PxTriangleMeshCookingResult::Enum result;
//			myCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

//			DirectX::SimpleMath::Vector3 translation;
//			DirectX::SimpleMath::Vector3 scale;
//			DirectX::SimpleMath::Quaternion quat;
//			DirectX::SimpleMath::Matrix transform = gameObjectsToCook[i]->GetComponent<CInstancedModelComponent>()->GetInstancedTransforms()[z];
//			transform.Decompose(scale, quat, translation);

//			PxMeshScale meshScale(PxVec3(scale.x, scale.y, scale.z), PxQuat(PxIdentity));

//			PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//			PxTriangleMesh* pxMesh = myPhysics->createTriangleMesh(readBuffer);

//			PxTriangleMeshGeometry pMeshGeometry(pxMesh, meshScale);

//			PxRigidStatic* actor = myPhysics->createRigidStatic({ 0.f, 0.f, 0.f });
//			actor->userData = (void*)gameObjectsToCook[i]->myTransform;
//			PxShape* instancedShape = myPhysics->createShape(pMeshGeometry, *CreateMaterial(CPhysXWrapper::materialfriction::basic), true);

//			PxFilterData filterData;
//			filterData.word0 = CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT;
//			instancedShape->setQueryFilterData(filterData);
//			actor->attachShape(*instancedShape);
//			aScene->PXScene()->addActor(*actor);


//			PxVec3 pos = { translation.x, translation.y, translation.z };
//			PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };
//			actor->setGlobalPose({ pos, pxQuat });
//		}
//	}
	/*else if (gameObjectsToCook[i]->GetComponent<CModelComponent>() && !gameObjectsToCook[i]->GetComponent<CPlayerControllerComponent>())
	{
		std::vector<PxVec3> verts(gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies.size());

		for (auto y = 0; y < gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies.size(); ++y) {
			Vector3 vec = gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myVertecies[y];
			verts[y] = PxVec3(vec.x, vec.y, vec.z);
		}

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = (PxU32)verts.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = verts.data();

		std::vector<unsigned int> myIndexes = gameObjectsToCook[i]->GetComponent<CModelComponent>()->GetMyModel()->GetModelData().myMeshFilter.myIndexes;
		meshDesc.triangles.count = (PxU32)myIndexes.size() / (PxU32)3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = myIndexes.data();

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;
		myCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* pxMesh = myPhysics->createTriangleMesh(readBuffer);

		PxTriangleMeshGeometry pMeshGeometry(pxMesh);

		PxRigidStatic* actor = myPhysics->createRigidStatic({ 0.f, 0.f, 0.f });
		PxShape* shape = myPhysics->createShape(pMeshGeometry, *myPXMaterial, true);
		actor->attachShape(*shape);

		DirectX::SimpleMath::Vector3 translation;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion quat;
		DirectX::SimpleMath::Matrix transform = gameObjectsToCook[i]->myTransform->GetLocalMatrix();
		transform.Decompose(scale, quat, translation);
		PxVec3 pos = { translation.x, translation.y, translation.z };
		PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };

		actor->setGlobalPose({ pos, pxQuat });

		aScene->PXScene()->addActor(*actor);
	}*/
	//}
//}
