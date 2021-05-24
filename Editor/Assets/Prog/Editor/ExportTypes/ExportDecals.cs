﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct Decal
{
    public int instanceID;
    public string materialName;

}
[System.Serializable]
public struct DecalCollection
{
    public List<Decal> decals;
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
        decalCollection.decals = new List<Decal>();

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
                    //GameObject asset = PrefabUtility.GetCorrespondingObjectFromOriginalSource(meshFilter).gameObject;
                    //if (asset == null)
                    //    continue;

                    Decal aDecal = new Decal();
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