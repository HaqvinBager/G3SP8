using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using UnityEditor;
using UnityEngine.SceneManagement;
using UnityEditor.SceneManagement;
using System.Text;

public class ExporterBin
{
    //private static string Base { get => "Assets/Generated/"; }

    public BinaryWriter binWriter = null;

    public ExporterBin(string aFileName)
    {
        string rootFolder = MagicString.GeneratedPath;
        string folder = rootFolder + aFileName;

        if (!Directory.Exists(Application.dataPath + folder))
        {
            Directory.CreateDirectory(rootFolder + aFileName);
        }
        
           
        AssetDatabase.Refresh();

        binWriter = new BinaryWriter(new FileStream(folder + "\\" + aFileName + ".bin", FileMode.Create));
    }


    //[MenuItem("Export/Export Bin")]
    public static void Export()
    {
        Scene activeScene = EditorSceneManager.GetActiveScene();
        DirectoryInfo directoryInfo = new DirectoryInfo(activeScene.path);
        string fullName = directoryInfo.Parent.FullName;
        string levelFolder = fullName.Substring(fullName.IndexOf("Assets"), fullName.Length - fullName.IndexOf("Assets"));
        LevelCollection level = AssetDatabase.LoadAssetAtPath<LevelCollection>(levelFolder + "\\" + directoryInfo.Parent.Name + "_Scenes.asset");

        StringBuilder infoLog = new StringBuilder();

        infoLog.AppendLine("[Binary Export] - " + level.name + "\n");

        var ids = ExportInstanceID.Export(level.name);
        List<int> exportedInstanceIDs = new List<int>();
        ids.Ids.ForEach(e => exportedInstanceIDs.Add(e.instanceID));

        ExporterBin exporter = new ExporterBin(directoryInfo.Parent.Name);
       
        exporter.binWriter.Write(ids);
        infoLog.AppendLine(exporter.binWriter.Write(ExportTransform.Export(level.name, exportedInstanceIDs)) + "\n");
        infoLog.AppendLine(exporter.binWriter.Write(ExportModel.Export(level.name, exportedInstanceIDs)));
        exporter.binWriter.Write(ExportPointlights.ExportPointlight(level.name));
        exporter.binWriter.Write(ExportSpotLight.Export());
        exporter.binWriter.Write(ExportCollider.Export(level.name, exportedInstanceIDs));
        infoLog.AppendLine(exporter.binWriter.Write(ExportInstancedModel.Export(level.name)));

        Player playerData = ExporterJson.ExportPlayer(level.name);
        if(ExportEndEvent.TryExport(playerData.player, out EndEventCollection outData))
        {
            exporter.binWriter.Write(1);
            exporter.binWriter.Write(outData);
        }
        else
        {
            exporter.binWriter.Write(0);
        }

        ListenerCollection listeners = ExportListener.Export();
        listeners.Export(exporter);

        KeyCollection keys = ExportKey.Export();
        keys.Export(exporter);

        exporter.binWriter.Close();
        Debug.Log(infoLog.ToString(), level);
    }
}
