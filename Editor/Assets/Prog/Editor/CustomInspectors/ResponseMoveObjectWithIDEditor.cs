using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CanEditMultipleObjects]
[CustomEditor(typeof(ResponseMoveObjectWithID))]
public class ResponseMoveObjectWithIDEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        
        this.ShowValueAndButton<ResponseMoveObjectWithID>("start", "Save Start", SavePosition);
        this.ShowValueAndButton<ResponseMoveObjectWithID>("end", "Save End", SavePosition);
        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<ResponseMoveObjectWithID>("start", "Move To Start", SetPosition);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<ResponseMoveObjectWithID>("end", "Move To End", SetPosition);
        serializedObject.ApplyModifiedProperties();
    }

    void SetPosition(Vector3 aValue)
    {
        var move = (ResponseMove)target;
        move.transform.position = aValue;
    }

    void SavePosition(SerializedProperty aProperty, Vector3 aValue)
    {
        aProperty.vector3Value = aValue;
    }
}
