using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(Move))]
public class MoveEditor : Editor
{

    SerializedProperty startProp;
    SerializedProperty endProp;
    SerializedProperty durationProp;

    void OnEnable()
    {
        startProp = serializedObject.FindProperty("start");
        endProp = serializedObject.FindProperty("end");
        durationProp = serializedObject.FindProperty("duration");
    }

    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        
        this.Vector3SetPosition<Move>("start", "Save Start");
        this.Vector3SetPosition<Move>("end", "Save End");
        GUI.backgroundColor = Color.green;
        this.SetPositionToVector3<Move>("start", "Move To Start");
        GUI.backgroundColor = Color.cyan;
        this.SetPositionToVector3<Move>("end", "Move To End");
        serializedObject.ApplyModifiedProperties();
    }
}
