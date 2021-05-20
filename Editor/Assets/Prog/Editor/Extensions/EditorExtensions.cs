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
}
