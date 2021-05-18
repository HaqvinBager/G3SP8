using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class MagicString
{
    public const string GeneratedPath = "Assets/Generated/";

    public static List<Texture> GetTextures()
    {
        return LoadAssets<Texture>("t:Texture", "Assets/IronWrought/Texture");

        //List<Texture> textures =// new List<Texture>();
        ////string[] guids = GetGuids("t:texture", "Assets/IronWrought/Texture");
        ////foreach(string guid in guids)
        ////{
        ////    textures.Add(LoadAsset<Texture>(guid));
        ////}
        //return textures;
    }

    public static List<T> LoadAssets<T>(string aTypeFilter, params string[] searchInFolders) where T : Object
    {
        List<T> objects = new List<T>();
        string[] guids = GetGuids(aTypeFilter, searchInFolders);
        foreach(string guid in guids)
        {
            objects.Add(LoadAsset<T>(guid));
        }
        return objects;
    }  

    public static T LoadAsset<T>(string aGuid) where T : Object
    {
        return AssetDatabase.LoadAssetAtPath<T>(AssetDatabase.GUIDToAssetPath(aGuid));
    }

    public static string[] GetGuids(string aFilter = "t:texture", params string[] someFolders)
    {
        return AssetDatabase.FindAssets(aFilter, someFolders);
    }
}



