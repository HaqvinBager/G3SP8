#pragma once
#include <pxPhysicsApi.h>

class CContactFilterCallback : public physx::PxSimulationFilterCallback
{
public:
	CContactFilterCallback() = default;
	~CContactFilterCallback() = default;
	physx::PxFilterFlags	pairFound(physx::PxU32 pairID,
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, const physx::PxActor* a0, const physx::PxShape* s0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, const physx::PxActor* a1, const physx::PxShape* s1,
		physx::PxPairFlags& pairFlags) override;
	void			pairLost(physx::PxU32 pairID,
		physx::PxFilterObjectAttributes attributes0,
		physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1,
		physx::PxFilterData filterData1,
		bool objectRemoved) override;
	bool			statusChange(physx::PxU32& pairID, physx::PxPairFlags& pairFlags, physx::PxFilterFlags& filterFlags) override;
private:
};

