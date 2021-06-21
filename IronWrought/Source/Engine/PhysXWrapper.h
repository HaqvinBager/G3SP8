#pragma once
#include <PxPhysicsAPI.h>
#include <queue>

using namespace physx;

class CContactReportCallback;
class CRigidDynamicBody;
class CScene;
class CCharacterController;
class CDynamicRigidBody;
class CCharacterReportCallback;
class CContactFilterCallback;
class CModel;

class CPhysXWrapper
{
public:

	enum class materialfriction
	{
		metal,
		wood,
		bounce,
		basic,
		none
	};


	enum ELayerMask : physx::PxU32
	{
		STATIC_ENVIRONMENT = (1 << 6),
		DYNAMIC_OBJECTS = (1 << 7),
		WORLD = STATIC_ENVIRONMENT | DYNAMIC_OBJECTS,	

		ENEMY = (1 << 8),
		PLAYER = (1 << 9),
		COVER = (1 << 10),
		//Om man l�gger tilll ELayerMask Layers vill man addera dom nedan s� att "all" alltid representerar alla layers //Axel Savage 2021-05-07
		ALL = STATIC_ENVIRONMENT | DYNAMIC_OBJECTS | PLAYER | ENEMY | COVER /* | NyttLayer  */ //Axel Savage 2021-05-07
	};

public:

	CPhysXWrapper();
	~CPhysXWrapper();

	bool Init();
	void Simulate();

	PxScene* CreatePXScene();
	PxScene* GetPXScene();
	PxPhysics* GetPhysics() { return myPhysics; }


	bool TryRayCast(const Vector3& aOrigin, Vector3& aDirection, const float aDistance, PxRaycastBuffer& outHit);

	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance, ELayerMask aLayerMask = ELayerMask::ALL);
	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance, physx::PxU32 aLayerMask);

	PxMaterial* CreateMaterial(materialfriction amaterial);
	PxMaterial* CreateCustomMaterial(const float& aDynamicFriction, const float& aStaticFriction, const float& aBounciness);
	CRigidDynamicBody* CreateDynamicRigidbody(const CTransformComponent& aTransform);
	CRigidDynamicBody* CreateDynamicRigidbody(const PxTransform& aTransform);

	CCharacterController* CreateCharacterController(const Vector3& aPos, const float& aRadius, const float& aHeight, CTransformComponent* aUserData = nullptr, physx::PxUserControllerHitReport* aHitReport = nullptr);

	PxControllerManager* GetControllerManager();

	physx::PxTransform ConvertToPxTransform(const Matrix& aTransform);

	physx::PxUserControllerHitReport* GetPlayerReportBack() { return myPlayerReportCallback; }


  //merge conflict 8/3/2021
	//void DebugLines();
	void Cooking(const std::vector<CGameObject*>& gameObjectsToCook, CScene* aScene);
	physx::PxShape* CookObject(CGameObject& aGameObject);


private:

	PxConvexMeshDesc GetConvexMeshDesc(const std::vector<PxVec3>& someVerts, const std::vector<unsigned int>& someIndices, const PxConvexFlags someFlags) const;
	PxTriangleMeshDesc GetTriangleMeshDesc(const std::vector<PxVec3>& someVerts, const std::vector<unsigned int>& someIndices) const;
	PxShape* CookShape(const CModel* aModel, const CTransformComponent* aTransform, const physx::PxMaterial* aMaterial) const;
	std::vector<PxRigidStatic*> CookShapes(const CModel* aModel, const CTransformComponent* aTransform, const physx::PxMaterial* aMaterial, const std::vector<Matrix>& someTransforms) const;

private:
	PxFoundation* myFoundation;
	PxPhysics* myPhysics;
	PxDefaultCpuDispatcher* myDispatcher;
	PxMaterial* myPXMaterial;
	PxPvd* myPhysicsVisualDebugger;
	PxDefaultAllocator* myAllocator;
	PxCooking* myCooking;
	CContactReportCallback* myContactReportCallback;
	PxControllerManager* myControllerManager;
	physx::PxUserControllerHitReport* myPlayerReportCallback;
	CContactFilterCallback* myContactFilterReportCallback;;
	//std::queue<CRigidDynamicBody*> myAddBodyQueue;


private:
	template<typename From, typename To>
	struct Convert {
		void operator()(const std::vector<From>& aFrom, std::vector<To>& aTo)
		{ }
	};

	template<>
	struct Convert<Vector3, PxVec3> {
		void operator()(const std::vector<Vector3>& aFrom, std::vector<PxVec3>& aTo)
		{
			aTo.reserve(aFrom.size());
			for (const auto& vec : aFrom)
				aTo.push_back(PxVec3(vec.x, vec.y, vec.z));
		}
	};

	template<>
	struct Convert<const std::vector<Vector3>*, PxVec3> {
		void operator()(const std::vector<const std::vector<Vector3>*>& aFrom, std::vector<PxVec3>& aTo)
		{
			size_t size = 0;
			for (const auto& from : aFrom)
				size += from->size();

			aTo.reserve(size);
			for (const auto& from : aFrom)
				for (const auto& vec : *from)
					aTo.push_back(PxVec3(vec.x, vec.y, vec.z));
		}
	};

	template<>
	struct Convert<const std::vector<unsigned int>*, unsigned int> {
		void operator()(const std::vector<const std::vector<unsigned int>*>& aFrom, std::vector<unsigned int>& aTo)
		{
			size_t size = 0;
			for (const auto& from : aFrom)
				size += from->size();

			aTo.reserve(size);
			for (const auto& from : aFrom)
				for (const auto& vec : *from)
					aTo.push_back(vec);
		}
	};
};
