using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(CustomEvent))]
public class CustomEventEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        if (GUILayout.Button("Raise"))
        {
            var script = target as CustomEvent;
            script.OnRaiseEvent();
        }

    }
}