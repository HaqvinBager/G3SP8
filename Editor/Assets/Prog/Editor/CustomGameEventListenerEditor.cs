using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using System.Reflection;
using System;

#if UNITY_EDITOR
using UnityEditor;
#endif

[CustomEditor(typeof(CustomEventListener))]
public class CustomGameEventListenerEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
    }
}
