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
		STATIC_ENVIRONMENT = (1 << 0),
		DYNAMIC_OBJECTS = (1 << 1),
		WORLD = STATIC_ENVIRONMENT | DYNAMIC_OBJECTS,	

		PLAYER = (1 << 8),
		ENEMY = (1 << 9),

		//Om man l�gger tilll ELayerMask Layers vill man addera dom nedan s� att "all" alltid representerar alla layers //Axel Savage 2021-05-07
		ALL = STATIC_ENVIRONMENT | DYNAMIC_OBJECTS | PLAYER | ENEMY /* | NyttLayer  */ //Axel Savage 2021-05-07
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

	physx::PxUserControllerHitReport* GetPlayerReportBack() { return myPlayerReportCallback; }


  //merge conflict 8/3/2021
	//void DebugLines();
	void Cooking(const std::vector<CGameObject*>& gameObjectsToCook, CScene* aScene);
	physx::PxShape* CookObject(CGameObject& aGameObject);


private:

	template<typename From, typename To>
	struct Convert {
		void operator()(const std::vector<From>& aFrom, std::vector<To>& aTo)
		{

		}
	};

	template<>
	struct Convert<Vector3, PxVec3> {
		void operator()(const std::vector<Vector3>& aFrom, std::vector<PxVec3>& aTo)
		{
			aTo.reserve(aFrom.size());
			for (const auto& vec : aFrom)
			{
				aTo.push_back(PxVec3(vec.x, vec.y, vec.z));
			}
		}
	};


	//void FromVector3ToPxVec3(const std::vector<Vector3>& aData, std::vector<PxVec3>& outVector);
	PxConvexMeshDesc GetMeshDesc(const std::vector<PxVec3>& verts, const std::vector<unsigned int>& indexes, const PxConvexFlags someFlags);


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
	//std::queue<CRigidDynamicBody*> myAddBodyQueue;
};
