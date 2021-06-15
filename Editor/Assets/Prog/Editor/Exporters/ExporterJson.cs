using System.IO;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
using UnityEditor.SceneManagement;

[System.Serializable]
public struct PlayerID
{
    public int instanceID;
    public List<int> childrenIDs;
}

[System.Serializable]
public struct Player
{
    public PlayerID player;
}

public class ExporterJson
{
    [MenuItem("Export/Export")]
    public static void ExportJson()
    {
        if (!EditorSceneManager.SaveCurrentModifiedScenesIfUserWantsTo())
        {
            return;
        }

        if (GameObject.FindObjectOfType<PlayerSpawnPosition>() == null)
        {
            Debug.LogError("Export Cancelled. Please place a PlayerSpawnPosition Prefab in your Layout Scene");
            return;
        }

        DirectoryInfo parentDirectory = Directory.GetParent(SceneManager.GetActiveScene().path);
        ExporterBin.Export();

        NavMeshData navMesh = ExportNavMeshToObj.Export(parentDirectory.Name);
        Json.BeginExport(parentDirectory.Name, navMesh);
        List<GameObject> allScenesActiveObjects = GetAllOpenedSceneActiveObjects();
        for (int i = 0; i < SceneManager.sceneCount; ++i)
        {
            Json.BeginScene(SceneManager.GetSceneAt(i).name);
            DeactivateAndExportScene(i, allScenesActiveObjects);
            Json.EndScene();
        }

        Json.EndExport(parentDirectory.Name, parentDirectory.Name);

        foreach (var gameObject in allScenesActiveObjects)
            gameObject.SetActive(true);

        ExportResource.Export("Resources");

        Debug.Log("[Export Complete]");
    }

    private static void DeactivateAndExportScene(int aSceneIndex, List<GameObject> allScenesActiveObjects)
    {
        GameObject[] gameobjects = SceneManager.GetSceneAt(aSceneIndex).GetRootGameObjects();
        List<GameObject> activeobjects = new List<GameObject>();
        foreach (var gameobject in gameobjects)
        {
            if (allScenesActiveObjects.Contains(gameobject))
            {
                activeobjects.Add(gameobject);
                gameobject.SetActive(true);
            }
        }


        ExportASceneJson(SceneManager.GetSceneAt(aSceneIndex).name);

        foreach (var gameobject in activeobjects)
        {
            gameobject.SetActive(false);
        }

        AssetDatabase.Refresh();
    }

    private static void ExportASceneJson(string aSceneName)
    {
        //Next Step is to make sure that we only write the correct data to the correct Scene_Json.json!
        //Or maybe just leave it be tbh! This might just be a good enough of a solution!
        //Especially if we put these multiple .json files in a folder!
        InstanceIDCollection instanceIDs = ExportInstanceID.Export(aSceneName);
        Json.AddToExport(instanceIDs);

        List<int> ids = new List<int>();
        instanceIDs.Ids.ForEach(e => ids.Add(e.instanceID));

        Player playerData = ExportPlayer(aSceneName);

        Json.AddToExport(ExportVertexPaint.Export(aSceneName, ids));
        Json.AddToExport(ExportDirectionalLight.Export(aSceneName));
        Json.AddToExport(ExportDecals.Export(aSceneName));
        Json.AddToExport(playerData);
        Json.AddToExport(ExportBluePrint.Export(aSceneName));
        Json.AddToExport(EnemyExporter.Export(aSceneName));
        Json.AddToExport(ExportParents.Export(aSceneName));
        Json.AddToExport(ExportEventTrigger.Export(aSceneName));
        Json.AddToExport(HealthPickupExporter.Export(aSceneName));
        Json.AddToExport(ExportAudioSource.Export(aSceneName));
        Json.AddToExport(ExportVFX.Export(aSceneName));
        Json.AddToExport(ExportSpotLight.Export());
        Json.AddToExport(ExportEventListener.Export());


        //Json.AddToExport(ExportSafetyDoors.Export(aSceneName));
        //Json.AddToExport(ExportFusebox.Export(aSceneName));
        //Json.AddToExport(FusePickUpExporter.Export(aSceneName));
        
        Json.AddToExport(ExportLock.Export());
        Json.AddToExport(ExportKey.Export());
        Json.AddToExport(ExportListener.Export(), true);
        //Json.AddToExport(ExportEndEvent.Export(playerData.player), true);
        //Json.AddToExport(ExportLevelTeleporter.Export(aSceneName), true);
    }

    public static Player ExportPlayer(string aSceneName)
    {
        Player data = new Player();
        PlayerSpawnPosition player = GameObject.FindObjectOfType<PlayerSpawnPosition>();
        if (player != null)
        {
            data.player.instanceID = player.transform.GetInstanceID();
            data.player.childrenIDs = new List<int>();
            foreach (Transform child in player.transform)
                data.player.childrenIDs.Add(child.GetInstanceID());
        }
        return data;
    }

    private static List<GameObject> GetAllOpenedSceneActiveObjects()
    {
        List<GameObject> allScenesActiveObjects = new List<GameObject>();
        for (int i = 0; i < SceneManager.sceneCount; ++i)
        {
            GameObject[] gameobjects = SceneManager.GetSceneAt(i).GetRootGameObjects();

            foreach (var gameobject in gameobjects)
            {
                if (gameobject.activeInHierarchy)
                {
                    allScenesActiveObjects.Add(gameobject);
                    gameobject.SetActive(false);
                }
            }
        }

        return allScenesActiveObjects;
    }
}


//[MenuItem("GameObject/BluePrint/Add Patrol Point", validate = true)]
//static bool ValidateTest()
//{
//    if (Selection.gameObjects.Length == 1)
//    {
//        return Selection.gameObjects[0].name.Contains("BP_");
//    }
//    return false;
//}
////[MenuItem("3D Create/PatrolPosition")]
//[MenuItem("GameObject/BluePrint/Add Patrol Point", false, 0)]
//static void Test()
//{
//    PrefabUtility.InstantiatePrefab(AssetDatabase.LoadAssetAtPath<Object>("Assets/Prefabs/PatrolPoint.prefab"), Selection.activeTransform);
//}