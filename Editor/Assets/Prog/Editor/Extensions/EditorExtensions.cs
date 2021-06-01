using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public static class EditorExtensions 
{
    public static void Ping(this Object aObject)
    {
        EditorGUIUtility.PingObject(aObject);
    }



    public static bool Vector3SetPosition<T>(this Editor anEditor, string memberName, string buttonLabel) where T : Component
    {
        bool buttonUsed = false;
        SerializedProperty prop = anEditor.serializedObject.FindProperty(memberName);
        EditorGUILayout.BeginHorizontal();
        EditorGUILayout.Vector3Field("", prop.vector3Value);
        if (GUILayout.Button(buttonLabel))
        {
            T script = (T)anEditor.target;
            prop.vector3Value = script.transform.position;
            buttonUsed = true;
        }
        EditorGUILayout.EndHorizontal();
        return buttonUsed;
    }

    public static bool SetPositionToVector3<T>(this Editor anEditor, string vec3MemberName, string buttonLabel) where T : Component
    {
        bool buttonUsed = false;
        SerializedProperty prop = anEditor.serializedObject.FindProperty(vec3MemberName);
        if (GUILayout.Button(buttonLabel))
        {
            T script = (T)anEditor.target;
            script.transform.position = prop.vector3Value;
            buttonUsed = true;
        }
        return buttonUsed;
    }

}
