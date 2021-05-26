﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System;

[CustomEditor(typeof(IronEvent))]
public class IronEventEditor : Editor
{
    List<string> gameEventTypes = new List<string>
    {
        typeof(GameEvent).ToString(),
        typeof(GameEventVector3).ToString()
    };

    SerializedProperty eventType = null;
    SerializedProperty eventProperty = null;
    string eventName = string.Empty;
    SerializedProperty eventFilter = null;
    SerializedProperty audioIndex = null;
    SerializedProperty triggerOnce = null;

    private void OnEnable()
    {
        eventType = serializedObject.FindProperty("eventType");
    }

    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        //EditorGUILayout.PropertyField(eventFilter);
        ////EEventFilter activeFilter = (EEventFilter)eventFilter.enumValueIndex;
        eventFilter = serializedObject.FindProperty("eventFilter");
        EditorGUILayout.PropertyField(eventFilter);

        EditorGUILayout.PropertyField(eventType);

        IronEventType activeEvent = (IronEventType) eventType.enumValueIndex;
        switch (activeEvent)
        {
           // case IronEventType.SoundEffectEvent:
           //     OnInspectorGUISoundEffects();
           //     break;
            case IronEventType.GameEvent:
                OnInspectorGUIGameEvents();
                break;

            case IronEventType.Move:
                OnInspectorGUIMoveEvent();
                break;

            //case IronEventType.VFXEvent:
            //    OnInspectorGUIVFXEvents();
            //    break;
        }

        audioIndex = serializedObject.FindProperty("audioIndex");
        EditorGUILayout.PropertyField(audioIndex);

        triggerOnce = serializedObject.FindProperty("triggerOnce");
        EditorGUILayout.PropertyField(triggerOnce);

        serializedObject.ApplyModifiedProperties();
    }

    private void OnInspectorGUIMoveEvent()
    {
        throw new NotImplementedException();
    }

    private void OnInspectorGUISoundEffects()
    {

    }
    private void OnInspectorGUIGameEvents()
    {
        eventProperty = serializedObject.FindProperty("eventObject");
        EditorGUILayout.PropertyField(eventProperty);
        
        if(eventProperty.objectReferenceValue == null)
        {
            EditorGUILayout.Space();
            eventName = EditorGUILayout.TextField(eventName);
    
            if(GUILayout.Button("Create Event"))
            {
                if (!AssetDatabase.IsValidFolder("Assets/GameEvents"))
                {
                    Debug.Log("Creating GUID: " + AssetDatabase.GUIDToAssetPath(AssetDatabase.CreateFolder("Assets", "GameEvents")));
                    AssetDatabase.Refresh();
                }
                GameEvent newEvent = ScriptableObject.CreateInstance<GameEvent>();
                newEvent.name = eventName;
                Debug.Log(eventName);
                AssetDatabase.CreateAsset(ScriptableObject.CreateInstance<GameEvent>(), "Assets/GameEvents/" + eventName + ".asset");
                eventProperty.objectReferenceValue = AssetDatabase.LoadAssetAtPath<GameEvent>("Assets/GameEvents/" + eventName + ".asset");
                eventName = "event name";
            }
        }
    }
    private void OnInspectorGUIVFXEvents()
    {
        var start = serializedObject.FindProperty("start");
        var end = serializedObject.FindProperty("end");



    }
}
