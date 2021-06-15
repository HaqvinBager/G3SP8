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
            SerializedProperty newElement = AddToPath();
            EndEvent script = (EndEvent)target;
            newElement.FindPropertyRelative("position").vector3Value = script.transform.ConvertToIronWroughtPosition();
            newElement.FindPropertyRelative("rotation").quaternionValue = script.transform.ConvertToIronWroughtRotationQuaternion();
            newElement.FindPropertyRelative("duration").floatValue = 0.5f;
        }
        serializedObject.ApplyModifiedProperties();
    }

    private SerializedProperty AddToPath()
    {
        SerializedProperty list = serializedObject.FindProperty("path");
        list.arraySize++;
        return list.GetArrayElementAtIndex(list.arraySize - 1);
    }
}
