using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
#if UNITY_EDITOR
using UnityEditor;
#endif

[System.Serializable]
public struct ModelAsset
{
    public int id;
    public string path;
}

[System.Serializable]
public struct VertexColorAsset
{
    public int id;
    public string path;
}

[System.Serializable]
public struct TextureAsset
{
    public int id;
    public string path;
}

[System.Serializable]
public struct MaterialAsset
{
    public int id;
    public string name;
    public List<int> textureAssets;
}

[System.Serializable]
public struct EventAsset
{
    public int id;
    public string eventName;
}


[System.Serializable]
public struct Assets
{
    public List<ModelAsset> models;
    public List<VertexColorAsset> vertexColors;
    public List<TextureAsset> textures;
    public List<MaterialAsset> materials;
    public List<EventAsset> events;
}

public class ExportResource 
{
    public static void Export(string aSceneName)
    {
        ExportModelAssets(aSceneName);
    }


    public static Assets ExportModelAssets(string aSceneName)
    {
        string[] allAssetPaths = AssetDatabase.GetAllAssetPaths();
        Assets assets = new Assets();
        assets.models = new List<ModelAsset>();
        assets.vertexColors = new List<VertexColorAsset>();
        assets.textures = new List<TextureAsset>();
        assets.materials = new List<MaterialAsset>();
        assets.events = new List<EventAsset>();

        List<Object> models = MagicString.LoadAssets<Object>("t:Model", "Assets/IronWrought/Mesh");
        Debug.Log(models.Count);
        foreach(GameObject model in models)
        {
            ModelAsset modelAsset = new ModelAsset();
            modelAsset.id = model.transform.GetInstanceID();
            modelAsset.path = AssetDatabase.GetAssetPath(model);
            assets.models.Add(modelAsset);
        }

        List<Texture> textures = MagicString.GetTextures();
        foreach (Texture t in textures)
        {
            TextureAsset asset = new TextureAsset();
            asset.id = t.GetInstanceID();
            asset.path = AssetDatabase.GetAssetPath(t);
            assets.textures.Add(asset);
        }

        List<Material> materials = MagicString.LoadAssets<Material>("t:Material", "Assets/Material");
        foreach(Material mat in materials)
        {
            MaterialAsset asset = new MaterialAsset();
            asset.id = mat.GetInstanceID();
            asset.name = mat.name;
            asset.textureAssets = new List<int>();

            List<string> texturePaths = MagicString.ExtractIronWroughtTexturePathFromMaterial(mat);
            foreach(string texturePath in texturePaths)
            {
                Texture texture = AssetDatabase.LoadAssetAtPath<Texture>(texturePath);
                asset.textureAssets.Add(texture.GetInstanceID());
            }

            assets.materials.Add(asset);
        }

        //List<IronEvent> events = MagicString.LoadAssets<IronEvent>("l:IronEvent", "Assets/CustomEvents");
        //foreach(IronEvent ironEvent in events)
        //{
        //    Debug.Log(ironEvent?.name);
        //    //EventAsset asset = new EventAsset();
        //    //asset.id = ironEvent.GetInstanceID();
        //    //asset.eventName = ironEvent.name;
        //    //assets.events.Add(asset);
        //}

        foreach (string assetPath in allAssetPaths)
        {
            if (assetPath.Contains("/Generated/"))
            {
                if (assetPath.Contains("/VertexColors/"))
                {
                    Object asset = AssetDatabase.LoadAssetAtPath<Object>(assetPath);
                    VertexColorAsset vertexColorAsset = new VertexColorAsset();
                    vertexColorAsset.id = asset.GetInstanceID();
                    vertexColorAsset.path = assetPath;
                    assets.vertexColors.Add(vertexColorAsset);
                }
            }
        }

        Json.ExportToJson(assets, "Resource");
        return assets;
    }

    //public static void ExportBluePrintPrefabs(Scene aScene)
    //{
    //    BluePrintPrefabs collection = new BluePrintPrefabs();
    //    collection.blueprintPrefabs = new List<BluePrintAsset>();
    //    var prefabs = ExportBluePrint.LoadBluePrintPrefabGameObjects();
    //    foreach (var prefab in prefabs)
    //    {
    //        BluePrintAsset link = new BluePrintAsset();
    //        link.id = prefab.transform.GetInstanceID();
    //        link.type = prefab.name;
    //        link.childCount = prefab.transform.childCount;

    //        collection.blueprintPrefabs.Add(link);
    //    }

    //    Json.ExportToJson(collection, "Resource");
    //}


}
