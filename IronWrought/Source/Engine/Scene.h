#pragma once
#include <vector>
#include "GameObject.h"
#include "Component.h"
#include "SpriteInstance.h"
#include "EngineDefines.h"
#include "PhysXWrapper.h"
#include "PostMaster.h"

class CModelComponent;
class CCamera;
class CEnvironmentLight;
class CCollisionManager;
class CPointLight;
class CSpotLight;
class CBoxLight;
class CLineInstance;
class CCanvas;

class CAnimatedUIElement;
class CTextInstance;
class CCameraComponent;
struct SLineTime;
struct SNavMesh;
class CModel;
class IAIBehavior;
class CInstancedModelComponent;
class CPatrolPointComponent;

class CPlayerControllerComponent;

typedef std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> LightPair;

enum class ESceneCamera {
	PlayerFirstPerson,
	FreeCam,
	MenuCam,
	UnlockCursor,
	NoCamera
};

class CScene : public IObserver {
	friend class CEngine;
	friend class CBootUpState;
	friend class CInGameState;
	friend class CMainMenuState;
	friend class CPauseMenuState;
public:
//SETUP START
	CScene(const int aNumberOfSections = 1, const unsigned int aGameObjectCount = 0);
	~CScene();

	//static CScene* GetInstance();
	bool Init();
	bool InitNavMesh(const std::string& aPath);

	bool InitCanvas(const std::string& aPath);
	bool InitCanvas();
	bool ReInitCanvas(const std::string& aPath, const bool& aDelete = false);
	void CanvasIsHUD(const bool& aIsHud = true);
	void DisableWidgetsOnCanvas();
	void CanvasToggle(const bool& anIsEnabled, const bool& anIsForceEnable = false);
	// Sets Canvas in Scene to use a Canvas owned by another object.
	void SetCanvas(CCanvas* aCanvas);
	void UpdateOnlyCanvas(const bool anUpdateOnlyCanvas);

//SETUP END

//UPDATE
public:
	void Awake();
	void Start();
	void Update();
	void FixedUpdate();
	void Receive(const SMessage& aMessage) override;

private:
	void InitAnyNewComponents();
	void CallStartOnNewComponents();
	void CallAwakeOnNewComponents();
//UPDATE END

public:
//SETTERS START
	void AddCamera(CCameraComponent* aCamera, const ESceneCamera aCameraType);
	void MainCamera(const ESceneCamera aCameraType);
	void Player(CGameObject* aPlayerObject);
	void AddEnemyShortcut(CGameObject* anEnemyObject, const int aSection = 0);
	bool EnvironmentLight(CEnvironmentLight* anEnvironmentLight);
	void ShouldRenderLineInstance(const bool aShouldRender);
#ifdef _DEBUG
	bool ShouldRenderLineInstance() { return myShouldRenderLineInstance; }
#endif
	
//SETTERS END
public:
	template <class T>
	T* FindFirstObjectWithComponent() {
		for (auto& gameObjects : myGameObjects)
		{
			for (auto& gameObject : gameObjects) {
				if (gameObject->GetComponent<T>() != nullptr) {
					return gameObject->GetComponent<T>();
				}
			}
		}
		return nullptr;
	}
	template <class T>
	T* FindFirstObjectWithComponentInSection(const int aInSection = 0) {
		if (aInSection >= 0 && aInSection < myGameObjects.size())
		{
			for (auto& gameObject : myGameObjects[aInSection]) {
				if (gameObject->GetComponent<T>() != nullptr) {
					return gameObject->GetComponent<T>();
				}
			}
		}
		return nullptr;
	}
public:
//GETTERS START
	CCameraComponent* MainCamera();
	CGameObject* Player();
	CGameObject* GetEnemyInSection(const int aSection = 0);
	CPlayerControllerComponent* PlayerController();
	CEnvironmentLight* EnvironmentLight();
	SNavMesh* NavMesh();
	PxScene* PXScene();
	std::vector<CGameObject*> ModelsToOutline() const;
	std::vector<CPointLight*>& PointLights();
	CCanvas* Canvas();
	const std::vector<CGameObject*>& ActiveGameObjects() const;
	const std::vector<CPatrolPointComponent*>& PatrolPoints() const;
//GETTERS END
public:
	const bool Ready() const { return myIsReadyToRender; }
	void Ready(bool aReady) { myIsReadyToRender = aReady; }
public:
//CULLING START
	std::vector<CEnvironmentLight*> CullSecondaryEnvironmentLights(CGameObject* aGameObject);
	std::vector<CPointLight*> CullPointLights(CGameObject* aGameObject);
	std::vector<CSpotLight*> CullSpotLights(CGameObject* aGameObject);
	std::vector<CBoxLight*> CullBoxLights(CGameObject* aGameObject);
	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> CullLights(CGameObject* aGameObject);
	const std::vector<CLineInstance*>& CullLineInstances() const;
	const std::vector<SLineTime>& CullLines() const;
	LightPair CullLightInstanced(CInstancedModelComponent* aModelType);
	std::vector<CGameObject*> CullGameObjects(CCameraComponent* aMainCamera);
	CGameObject* FindObjectWithID(const int aGameObjectInstanceID);
	CGameObject* FindObjectWithTag(const std::string aTag);
	std::vector<CGameObject*>* FindObjectsWithTag(const std::string aTag);
	template<class T>
	std::vector<CComponent*>* GetAllComponents();
//CULLING END
public:
	//POPULATE SCENE START

	// Note that this is used for environmentlights OTHER than the main one
	bool AddInstance(CEnvironmentLight* aSecondaryDirectionalLight);
	bool AddInstance(CPointLight* aPointLight);
	bool AddInstance(CSpotLight* aSpotLight);
	bool AddInstance(CBoxLight* aBoxLight);
	bool AddInstance(CLineInstance* aLineInstance);
	bool AddInstance(CGameObject* aGameObject, const int aToASection = -1);
	bool AddInstances(std::vector<CGameObject*>& someGameObjects, const int aToASection = -1);
	bool AddInstance(CPatrolPointComponent* aPatrolComponent);
	//PhysX
	bool AddPXScene(PxScene* aPXScene);
	//POPULATE SCENE END
public:
//REMOVE SPECIFIC INSTANCE START
	bool RemoveInstance(CEnvironmentLight* aSecondaryEnvironmentLight);
	bool RemoveInstance(CPointLight* aPointLight);
	bool RemoveInstance(CSpotLight* aSpotLight);
	bool RemoveInstance(CBoxLight* aBoxLight);
	bool RemoveInstance(CGameObject* aGameObject);
//REMOVE SPECIFIC INSTANCE END
//CLEAR SCENE OF INSTANCES START
	bool ClearSecondaryEnvironmentLights();
	bool ClearPointLights();
	bool ClearSpotLights();
	bool ClearBoxLights();
	bool ClearLineInstances();
	bool ClearGameObjects();
//CLEAR SCENE OF INSTANCES START


public:
	void AddSection(const bool aSetToCurrent = false);
	void SetNumberOfSections(const int aNumberOfSections);
	void NextSection(const bool aShouldToggle = false);
	void PreviousSection(const bool aShouldToggle = false);
	void SetCurrentSection(const int aSection);
	// Enables this section, disables all others. Player is unchanged.
	void ToggleSections(const int aSection);
	void EnableSection(const int aSection);
	// Disables this section. Player is unchanged.
	void DisableSection(const int aSection);
	void EnableCurrent();
	void DisableCurrent();
	void ToggleToCurrent();
	const int& CurrentSection() const { return myCurrentSection; }

private:
	bool SectionBoundsCheck(const int aSection);
	
private:
	//Struct left because it might be needed later
	struct NearestPlayerComparer {
		DirectX::SimpleMath::Vector3 myPos;
		bool operator()(const CPointLight* a, const CPointLight* b) const;
	} ourNearestPlayerComparer;
private:
//CONTAINERS START
	std::vector<CEnvironmentLight*> mySecondaryEnvironmentLights;
	std::vector<CPointLight*> myPointLights;
	std::vector<CSpotLight*> mySpotLights;
	std::vector<CBoxLight*> myBoxLights;
	std::vector<CLineInstance*> myLineInstances;
	std::vector<std::vector<CGameObject*>> myGameObjects;
	std::vector<CGameObject*> myModelsToOutline;
	std::vector<CPatrolPointComponent*> myPatrolPoints;
	std::unordered_map<int, CGameObject*> myIDGameObjectMap; 
	std::unordered_map<std::string, std::vector<CGameObject*>> myGameObjectTagMap;
	std::unordered_map<size_t, std::vector<CComponent*>> myComponentMap;
	int myCurrentSection;//Used as index for myGameObjects, (more?)

	std::queue<CComponent*> myAwakeComponents;
	std::queue<CComponent*> myStartComponents;
//CONTAINERS END

private:
//POINTERS START
	CEnvironmentLight* myEnvironmentLight;
	SNavMesh* myNavMesh;
	CLineInstance* myNavMeshGrid;


	ESceneCamera myActiveCamera;
	CCameraComponent* myMainCamera;
	std::unordered_map<ESceneCamera, CCameraComponent*> myCameras;

	PxScene* myPXScene;
	CGameObject* myPlayer;
	std::unordered_map<int, CGameObject*> myEnemies;
	CCanvas* myCanvas;
	bool myDeleteCanvas;
	bool myUpdateOnlyCanvas;
//POINTERS END

// VFX EDITOR START
public:
	void SetVFXTester(CGameObject* aGameObject)
	{
		myVFXTester = aGameObject;
	}
	CGameObject* GetVFXTester() const
	{
		return myVFXTester;
	}
private:
	CGameObject* myVFXTester = nullptr;
// VFX EDITOR END

	bool myIsReadyToRender;
#ifdef  _DEBUG
private:
	bool myShouldRenderLineInstance;
	CLineInstance* myGrid;
#endif //  _DEBUG
};

template<class T>
inline std::vector<CComponent*>* CScene::GetAllComponents()
{
	size_t hashCode = typeid(T).hash_code();
	if (myComponentMap.find(hashCode) == myComponentMap.end())
		return nullptr;
	return &myComponentMap[hashCode];
}
