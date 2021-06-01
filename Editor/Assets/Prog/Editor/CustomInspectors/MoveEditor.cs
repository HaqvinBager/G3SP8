using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(Move))]
public class MoveEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        
        this.ShowValueAndButton<Move>("start", "Save Start", SavePosition);
        this.ShowValueAndButton<Move>("end", "Save End", SavePosition);
        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<Move>("start", "Move To Start", SetPosition);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<Move>("end", "Move To End", SetPosition);
        serializedObject.ApplyModifiedProperties();
    }

    void SetPosition(Vector3 aValue)
    {
        var move = (Move)target;
        move.transform.position = aValue;
    }


    void SavePosition(SerializedProperty aProperty, Vector3 aValue)
    {
        aProperty.vector3Value = aValue;
    }

}
