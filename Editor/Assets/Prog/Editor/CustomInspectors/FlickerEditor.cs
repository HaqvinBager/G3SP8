using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(ResponseFlicker))]
public class FlickerEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();
        SerializedProperty minMaxIntensity = serializedObject.FindProperty("myMinMaxIntensity");
        float min = minMaxIntensity.vector2Value.x;
        float max = minMaxIntensity.vector2Value.y;
        EditorGUILayout.MinMaxSlider("MinMax Intensity", ref min, ref max, 0.0f, 100.0f);
        minMaxIntensity.vector2Value = new Vector2(min, max);
        serializedObject.ApplyModifiedProperties();
    }
}
