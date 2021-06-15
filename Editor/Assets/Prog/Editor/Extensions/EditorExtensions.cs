using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using Object = UnityEngine.Object;

#if UNITY_EDITOR
using UnityEditor;
#endif

public static class EditorExtensions
{
    public static void Ping(this Object aObject)
    {
        EditorGUIUtility.PingObject(aObject);
    }

    public static bool ShowValueAndButton<T>(this Editor anEditor, string vector3MemberName, string buttonLabel, Action<SerializedProperty, Vector3> aCallback) where T : Component
    {
        bool buttonUsed = false;
        SerializedProperty prop = anEditor.serializedObject.FindProperty(vector3MemberName);
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PropertyField(prop);
        // EditorGUILayout.Vector3Field("", prop.vector3Value);
        if (GUILayout.Button(buttonLabel))
        {
            T script = (T)anEditor.target;
            aCallback?.Invoke(prop, script.transform.position);
            //prop.vector3Value = script.transform.position;
            buttonUsed = true;
        }
        EditorGUILayout.EndHorizontal();
        return buttonUsed;
    }

    public static bool ShowValueAndButton<T>(this Editor anEditor, string vector3MemberName, string buttonLabel, Action<SerializedProperty, Quaternion> aCallback) where T : Component
    {
        bool buttonUsed = false;
        SerializedProperty prop = anEditor.serializedObject.FindProperty(vector3MemberName);
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.PropertyField(prop);
        //EditorGUILayout.Vector3Field("", prop.vector3Value);
        if (GUILayout.Button(buttonLabel))
        {
            T script = (T)anEditor.target;
            aCallback?.Invoke(prop, script.transform.rotation);
            buttonUsed = true;
        }
        EditorGUILayout.EndHorizontal();
        return buttonUsed;
    }

    public static bool SetValueButtonCallback<T>(this Editor anEditor, string vector3MemberName, string buttonLabel, Action<Vector3> aCallback, bool viewVector3 = false) where T : Component
    {
        bool buttonUsed = false;
        SerializedProperty prop = anEditor.serializedObject.FindProperty(vector3MemberName);

        if (viewVector3)
        {
            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.PropertyField(prop);
            //EditorGUILayout.Vector3Field("", prop.vector3Value);
        }

        if (GUILayout.Button(buttonLabel))
        {
            aCallback(prop.vector3Value);
            buttonUsed = true;
        }

        if (viewVector3)
        {
            EditorGUILayout.EndHorizontal();
        }

        return buttonUsed;
    }



    public static bool SetValueButtonCallback<T>(this Editor anEditor, string vector3MemberName, string buttonLabel, Action<Quaternion> aCallback, bool viewVector3 = false) where T : Component
    {
        bool buttonUsed = false;
        SerializedProperty prop = anEditor.serializedObject.FindProperty(vector3MemberName);

        if (viewVector3)
        {
            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.Vector3Field("", prop.vector3Value);
        }

        if (GUILayout.Button(buttonLabel))
        {
            aCallback(Quaternion.Euler(prop.vector3Value));
            buttonUsed = true;
        }

        if (viewVector3)
        {
            EditorGUILayout.EndHorizontal();
        }

        return buttonUsed;
    }


    public static void SetPosition<T>(this Editor anEditor, Vector3 aVector3) where T : Component
    {
        T script = (T)anEditor.target;
        script.transform.position = aVector3;
    }
}
