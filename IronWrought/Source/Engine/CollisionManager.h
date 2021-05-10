#pragma once
//Ta hand om att skapa Colliders
//Registrera Colliders som blir skapade av AddComponent

//Kunna jämföra colliders om dom träffar varandra
//Ta emot en Ray och returnera vilket objekt eller vart rayen träffar första objektet.

namespace DirectX {
	namespace SimpleMath {
		struct Vector3;
		struct Vector2;
	}
}

class CCapsuleColliderComponent;
class CTriangleColliderComponent;
class CCircleColliderComponent;
class CRectangleColliderComponent;
class CGameObject;
class CCollider;

enum class ECollisionLayer {
	NONE				= 1 << 0,
	PLAYER				= 1 << 1,
	ENEMY				= 1 << 2,
	WALL				= 1 << 7,
	FLOOR				= 1 << 8,
	EVENT				= 1 << 9,
	ALL					= 1 << 10
};

class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

	void RegisterCollider(CCollider* aCollider);
	void ClearColliders();
	void Update();

private:
	std::vector<CCollider*> myColliders;

};