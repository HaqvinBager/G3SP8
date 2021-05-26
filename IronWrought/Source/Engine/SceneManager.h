#pragma once
#include "Scene.h"
#include "BinReader.h"
#include "JsonReader.h"
#include "StateStack.h"


#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>


struct SVertexPaintCollection;
class CBinReader;

enum class ColliderType {
	none = 0,
	BoxCollider = 1,
	SphereCollider = 2,
	CapsuleCollider = 3,
	MeshCollider = 4
};

//struct SSceneSetup {
//	concurrency::concurrent_vector<CGameObject*> myGameObjects;
//};

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(const std::string& aSceneName);
	static CScene* CreateMenuScene(const std::string& aCanvasPath);

private:
	static CScene* Instantiate();

	static bool AddGameObjects(CScene& aScene, RapidArray someData);
	static bool AddGameObjects(CScene& aScene, const std::vector<Binary::SInstanceID>& someData);

	//static void AddGameObjectNames(CScene& aScene, const std::vector<Binary::SInstanceName>& someNames);

	static void SetTransforms(CScene& aScene, RapidArray someData);
	static void SetTransforms(CScene& aScene, const std::vector<Binary::STransform>& someData);

	//static void AddModelComponents(CScene& aScene, RapidArray someData);
	static void AddModelComponents(CScene& aScene, const std::vector<Binary::SModel>& someData);

	static void AddPointLights(CScene& aScene, RapidArray someData);
	static void AddPointLights(CScene& aScene, const std::vector<Binary::SPointLight>& someData);
	static void AddSpotLights(CScene& aScene, const std::vector<Binary::SSpotLight>& someData);



	static void AddCollider(CScene& aScene, RapidArray someData);
	static void AddCollider(CScene& aScene, const std::vector<Binary::SCollider>& someData);

	//static void AddInstancedModelComponents(CScene& aScene, RapidArray someData);
	static void AddInstancedModelComponents(CScene& aScene, const std::vector<Binary::SInstancedModel>& someData);
	
	static void AddTriggerEvents(CScene& aScene, RapidArray someData);
	//static void AddTriggerEvents(CScene& aScene, const std::vector<Binary::SEventData>& someData);

	static void CreateCustomEvents(CScene& aScene);
	static void CreateCustomEventListeners(CScene& aScene);

	static void SetParents(CScene& aScene, RapidArray someData);
	
	static void SetVertexPaintedColors(CScene& aScene, RapidArray someData, const SVertexPaintCollection& vertexColorData);

	static void AddDirectionalLight(CScene& aScene, RapidObject someData);
	static void AddDirectionalLights(CScene& aScene, RapidArray someData);
	static void AddDecalComponents(CScene& aScene, RapidArray someData);
	static void AddPlayer(CScene& aScene, RapidObject someData);
	static void AddEnemyComponents(CScene& aScene, RapidArray someData);
	static void AddPickups(CScene& aScene, RapidArray someData);
	static void AddSafetyDoors(CScene& aScene, RapidArray someData);/*
	static void AddFuseboxes(CScene& aScene, RapidArray someData);
	static void AddFuses(CScene& aScene, RapidArray someData);*/
	static void AddAudioSources(CScene& aScene, RapidArray someData);
	static void AddVFX(CScene& aScene, RapidArray someData);
	static CScene* ourLastInstantiatedScene;
};


#include <future>
class CSceneFactory {
	
	friend class CEngine;
public:
	static CSceneFactory* Get();
	void LoadScene(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete = nullptr);
	void LoadSceneAsync(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete);

	void LoadSceneBin(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete = nullptr);
	void LoadSceneAsyncBin(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete);


	void Update();

private:
	CSceneFactory();
	~CSceneFactory();
	static CSceneFactory* ourInstance;

private:
	std::future<CScene*> myFuture;
	std::function<void(std::string)> myOnComplete;
	std::string myLastSceneName;
	CStateStack::EState myLastLoadedState;
	CBinReader* myBinReader;
};
