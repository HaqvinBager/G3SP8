using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(ResponseMove))]
public class ResponseMoveEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        
        this.ShowValueAndButton<ActivationMove>("start", "Save Start", SavePosition);
        this.ShowValueAndButton<ActivationMove>("end", "Save End", SavePosition);
        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<ActivationMove>("start", "Move To Start", SetPosition);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<ActivationMove>("end", "Move To End", SetPosition);
        serializedObject.ApplyModifiedProperties();
    }

    void SetPosition(Vector3 aValue)
    {
        var move = (ActivationMove)target;
        move.transform.position = aValue;
    }


    void SavePosition(SerializedProperty aProperty, Vector3 aValue)
    {
        aProperty.vector3Value = aValue;
    }

}
