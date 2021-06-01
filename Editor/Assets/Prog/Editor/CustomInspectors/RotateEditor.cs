using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(Rotate))]
public class RotateEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();

        this.ShowValueAndButton<Rotate>("start", "Rotation Start", SaveRotation);
        this.ShowValueAndButton<Rotate>("end", "Rotation End", SaveRotation);

        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<Rotate>("start", "Rotate to Start", SetRotation);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<Rotate>("end", "Rotate to End", SetRotation);
        
        serializedObject.ApplyModifiedProperties();
    }

    private void SetRotation(Quaternion quat)
    {
        var rotate = (Rotate)target;
        rotate.transform.rotation = quat;
    }

    private void SaveRotation(SerializedProperty aProperty, Quaternion aValue)
    {
        aProperty.vector3Value = aValue.eulerAngles;
    }
}
