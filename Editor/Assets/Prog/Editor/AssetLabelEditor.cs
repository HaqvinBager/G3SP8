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
            if (path.Contains("CustomEvents/"))
            {
                if (!AssetDatabase.IsValidFolder(path))
                {
                    Object asset = AssetDatabase.LoadAssetAtPath<Object>(path);
                    if(asset != null)
                        AssetDatabase.SetLabels(asset, labels.ToArray());
                }
            }
        }
    }
}
