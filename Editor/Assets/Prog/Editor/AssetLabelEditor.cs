using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class AssetLabelEditor 
{
    static List<string> labels = new List<string>
    {
        "IronEvent"
    };

    [MenuItem("Tools/Set Asset Labels [Custom Events]")]
    public static void SetLabels()
    {
        string[] assetPaths = AssetDatabase.GetAllAssetPaths();
        foreach (var path in assetPaths)
        {
            if (path.Contains("Assets/"))
            {
                if (!AssetDatabase.IsValidFolder(path))
                {
                    CustomEvent asset = AssetDatabase.LoadAssetAtPath<CustomEvent>(path);
                    if(asset != null)
                        AssetDatabase.SetLabels(asset, labels.ToArray());
                        //if(asset.GetType() == typeof(CustomEvent))
                }
            }
        }
    }
}
