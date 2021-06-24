using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(ActivationMoveObjectWithID))]
public class ActivationMoveObjectWithIDEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        
        this.ShowValueAndButton<ActivationMoveObjectWithID>("start", "Save Start", SavePosition);
        this.ShowValueAndButton<ActivationMoveObjectWithID>("end", "Save End", SavePosition);
        
        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<ActivationMoveObjectWithID>("start", "Move To Start", SetPosition);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<ActivationMoveObjectWithID>("end", "Move To End", SetPosition);
        serializedObject.ApplyModifiedProperties();
    }

    void SetPosition(Vector3 aValue)
    {
        var move = (ActivationMoveObjectWithID)target;
        move.transform.position = aValue;
    }


    void SavePosition(SerializedProperty aProperty, Vector3 aValue)
    {
        aProperty.vector3Value = aValue;
    }

}
