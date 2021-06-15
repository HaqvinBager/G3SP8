using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(EndEvent))]
public class EndEventEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        if(GUILayout.Button("Add Position & Rotation"))
        {
            SerializedProperty newElement = AddElement("path");
            EndEvent script = (EndEvent)target;
            newElement.FindPropertyRelative("position").vector3Value = script.transform.ConvertToIronWroughtPosition();
            newElement.FindPropertyRelative("rotation").quaternionValue = script.transform.ConvertToIronWroughtRotationQuaternion();
            newElement.FindPropertyRelative("duration").floatValue = 0.5f;
        }

        //if(GUILayout.Button("Add Empty VFX"))
        //{
        //    AddElement("vfx");       
        //}

        serializedObject.ApplyModifiedProperties();
    }

    private SerializedProperty AddElement(string aMemberName)
    {
        SerializedProperty list = serializedObject.FindProperty(aMemberName);
        list.arraySize++;
        return list.GetArrayElementAtIndex(list.arraySize - 1);
    }
}

[CustomEditor(typeof(VFXReference))]
public class VFXEditor : Editor
{
    int myChoice = 0;

    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        List<string> jsons = JsonFinder.GetJsonPaths("VFX");
        List<string> choices = new List<string>();
        foreach(string path in jsons)
        {
            choices.Add(path.Substring(path.LastIndexOf('/') + 1));
        }
       
        int choice = myChoice;
        myChoice = EditorGUILayout.Popup("VFX", myChoice, choices.ToArray());

        if(myChoice != choice)
        {
            string vfxString = jsons[myChoice].Substring(jsons[myChoice].LastIndexOf('_'));
            vfxString = vfxString.Remove(0, 1);
            //vfxString = vfxString.Substring(0, vfxString.Length);
            serializedObject.FindProperty("effectName").stringValue = vfxString;// jsons[myChoice].Substring('_');
        }
        serializedObject.ApplyModifiedProperties();

        choices.Clear();
    }

    private SerializedProperty AddElement(string aMemberName)
    {
        SerializedProperty list = serializedObject.FindProperty(aMemberName);
        list.arraySize++;
        return list.GetArrayElementAtIndex(list.arraySize - 1);
    }
}