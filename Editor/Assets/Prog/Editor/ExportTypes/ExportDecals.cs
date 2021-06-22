using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct DecalData
{
    public string materialName;
    public int instanceID;
    public int shouldRenderAlbedo;
    public int shouldRenderMaterial;
    public int shouldRenderNormals;
}
[System.Serializable]
public struct DecalCollection
{
    public List<DecalData> decals;
}

//static bool IsDecalMaterial(Material material)
//{
//    return material.shader.name.ToLower().Contains("decal"))
//}


public class ExportDecals
{
    public static DecalCollection Export(string aSceneName)
    {
        DecalCollection decalCollection = new DecalCollection();
        decalCollection.decals = new List<DecalData>();

        Renderer[] renderers = GameObject.FindObjectsOfType<Renderer>();

       

        foreach (Renderer decal in renderers)
        {
            if (!decal.sharedMaterial.shader.name.Contains("Decal"))
            {
                continue;
            }

            if (Json.TryIsValidExport(decal, out GameObject prefabParent))
            {
                if (decal.TryGetComponent(out MeshFilter meshFilter))
                {
                    DecalData aDecal = new DecalData();

                    if(prefabParent.TryGetComponent(out Decal obj))
                    {
                        aDecal.shouldRenderAlbedo = obj.renderAlbedo ? 1 : 0;
                        aDecal.shouldRenderMaterial = obj.renderMaterial ? 1 : 0;
                        aDecal.shouldRenderNormals = obj.renderNormal ? 1 : 0;
                    }
                    else
                    {
                        aDecal.shouldRenderAlbedo = 1;
                        aDecal.shouldRenderMaterial = 1;
                        aDecal.shouldRenderNormals = 1;
                    }

                    aDecal.materialName = ExtractMaterialName(decal.sharedMaterials);
                    aDecal.instanceID = prefabParent.transform.GetInstanceID();
                    decalCollection.decals.Add(aDecal);
                }
            }
        }
        return decalCollection;
        //Json.ExportToJson(decalList, aScene.name);
    }

    static string ExtractMaterialName(Material[] materials)
    {
        MaterialProperty[] matProperty = MaterialEditor.GetMaterialProperties(materials);
        for (int i = 0; i < matProperty.Length; ++i)
        {
            if (matProperty[i].displayName.ToString().Equals("Color"))
            {
                if (matProperty[i].textureValue != null)
                {
                    FileInfo fileInfo = new FileInfo(AssetDatabase.GetAssetPath(matProperty[i].textureValue));
                    return fileInfo.Directory.Name;
                }
            }
        }
        return "NA";
    }

}