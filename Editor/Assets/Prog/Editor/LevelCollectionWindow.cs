using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.SceneManagement;
using System.IO;

public class LevelCollectionWindow : EditorWindow
{
    [MenuItem("Export/Open Exporter")]
    public static void OnCreateWindow()
    {
        LevelCollectionWindow window = EditorWindow.CreateWindow<LevelCollectionWindow>();
        window.Show();
    }

    private Dictionary<LevelCollection, bool> myMultiExportToggles = null;
    private List<LevelCollection> myLevels = null;

    private void OnEnable()
    {
        myLevels = MagicString.LoadAssets<LevelCollection>("t:LevelCollection", "Assets/Scenes");
        myMultiExportToggles = new Dictionary<LevelCollection, bool>();
        foreach (LevelCollection level in myLevels)
        {
            myMultiExportToggles.Add(level, false);
        }
    }

    private void OnGUI()
    {
        foreach (LevelCollection level in myLevels)
        {
            if (level == null)
                continue;

            LevelCollectionRow(level);
        }

        if (GUILayout.Button("Export Selected"))
        {
            foreach (LevelCollection level in myLevels)
            {
                if (myMultiExportToggles.TryGetValue(level, out bool aShouldExport))
                {
                    if (aShouldExport)
                    {
                        ExportLevelCollection(level);
                    }
                }
            }
        }
    }

    private void LevelCollectionRow(LevelCollection aLevel)
    {
        Color originalColor = GUI.backgroundColor;
        SerializedObject serializedObject = new SerializedObject(aLevel);
        serializedObject.Update();

        EditorGUILayout.BeginHorizontal();

     
        EditorGUILayout.ObjectField("", aLevel, typeof(LevelCollection), false);
        OpenScenesButton(aLevel, originalColor);
        ConnectButton(aLevel, serializedObject, originalColor);
        ExportButton(aLevel);

        if (myMultiExportToggles.ContainsKey(aLevel))
            myMultiExportToggles[aLevel] = EditorGUILayout.Toggle(myMultiExportToggles[aLevel]);

        EditorGUILayout.EndHorizontal();

        serializedObject.ApplyModifiedProperties();
        GUI.backgroundColor = originalColor;
    }

    private void ExportButton(LevelCollection collection)
    {
        GUI.backgroundColor = collection.myScenes.Count > 0 ? Color.green : Color.red;
        if (GUILayout.Button("Export"))
        {
            ExportLevelCollection(collection);
        }
    }

    private void ExportLevelCollection(LevelCollection collection)
    {
        if (EditorSceneManager.SaveCurrentModifiedScenesIfUserWantsTo())
        {
            OpenScenes(collection);
            ExporterJson.ExportJson();
        }
    }

    private void OpenScenesButton(LevelCollection aLevel, Color originalColor)
    {
        GUI.backgroundColor = Color.yellow;
        if (GUILayout.Button("Open Level"))
        {
            OpenScenes(aLevel);
        }
        GUI.backgroundColor = originalColor;
    }

    private void OpenScenes(LevelCollection aLevel)
    {
        //LevelCollection levelCollection = (LevelCollection)target;
        EditorSceneManager.OpenScene(aLevel.myScenes[0].ScenePath, OpenSceneMode.Single);
        for (int i = 1; i < aLevel.myScenes.Count; ++i)
            EditorSceneManager.OpenScene(aLevel.myScenes[i].ScenePath, OpenSceneMode.Additive);
    }

    private void ConnectButton(LevelCollection collection, SerializedObject serializedObject, Color originalColor)
    {
        GUI.backgroundColor = GUI.backgroundColor = collection.myScenes.Count <= 0 ? Color.green : Color.Lerp(Color.cyan, Color.white, 0.25f);
        if (GUILayout.Button("Connect Scenes"))
        {
            string sceneFolder = AssetDatabase.GetAssetPath(collection);
            sceneFolder = sceneFolder.Substring(0, sceneFolder.LastIndexOf('/'));
            List<string> filePathsInFolder = new List<string>(Directory.EnumerateFiles(sceneFolder, "*.unity"));

            serializedObject.Update();
            var myScenes = serializedObject.FindProperty("myScenes");
            myScenes.ClearArray();
            serializedObject.ApplyModifiedProperties();

            foreach (var assetPath in filePathsInFolder)
            {
                string baseName = assetPath.Substring(assetPath.LastIndexOf('\\'), assetPath.Length - assetPath.LastIndexOf('\\'));
                baseName = baseName.Substring(1, baseName.Length - 1);
                string unityAssetPath = assetPath.Substring(assetPath.LastIndexOf('\\'), assetPath.Length - assetPath.LastIndexOf('\\'));
                unityAssetPath = unityAssetPath.Substring(1, baseName.Length - 1);

                if (!collection.myScenes.Exists(e => e.ScenePath.Contains(unityAssetPath)))
                {
                    SceneAsset sceneAsset = AssetDatabase.LoadAssetAtPath<SceneAsset>(assetPath);
                    RegisterScene(sceneAsset, serializedObject);
                }
            }
        }
        GUI.backgroundColor = originalColor;
    }

    private void RegisterScene(Object obj, SerializedObject serializedObject)
    {
        var myScenes = serializedObject.FindProperty("myScenes");
        int size = myScenes.arraySize;
        myScenes.InsertArrayElementAtIndex(size);
        var newElement = myScenes.GetArrayElementAtIndex(size);
        newElement.FindPropertyRelative("sceneAsset").objectReferenceValue = obj;
        obj = null;
    }
}
