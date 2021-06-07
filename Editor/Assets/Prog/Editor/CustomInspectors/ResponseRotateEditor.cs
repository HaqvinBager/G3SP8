using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(ResponseRotate))]
public class ResponseRotateEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();

        this.ShowValueAndButton<ActivationRotate>("start", "Rotation Start", SaveRotation);
        this.ShowValueAndButton<ActivationRotate>("end", "Rotation End", SaveRotation);

        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<ActivationRotate>("start", "Rotate to Start", SetRotation);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<ActivationRotate>("end", "Rotate to End", SetRotation);
        
        serializedObject.ApplyModifiedProperties();
    }

    private void SetRotation(Quaternion quat)
    {
        var rotate = (ActivationRotate)target;
        rotate.transform.rotation = quat;
    }

    private void SaveRotation(SerializedProperty aProperty, Quaternion aValue)
    {
        aProperty.vector3Value = aValue.eulerAngles;
    }
}