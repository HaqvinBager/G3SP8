using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
[CustomEditor(typeof(ResponseToggle))]
public class ToggleEditor : Editor 
{

    private List<System.Type> validTypes = new List<System.Type>()
    {
        typeof(Transform),
        typeof(PrefabProtector),
        typeof(MeshFilter),
        typeof(MeshRenderer),
        typeof(ResponseToggle),
        typeof(ActivationRotate),
        typeof(ActivationMove),
        typeof(Interest),
        typeof(Rigidbody)
    };

    bool foldOutOpen = false;
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();

        ResponseToggle script = (ResponseToggle)target;

        if(script.aTarget != null)
        {
            GameObject scriptTarget = script.aTarget.gameObject;

            //Component[] comps = scriptTarget.GetComponents<Component>();

            Component[] childComps = scriptTarget.GetComponentsInChildren<Component>();

            List<Component> components = new List<Component>();
            //foreach(Component comp in comps)
            //{
            //    if (CheckValidity(comp, validTypes.ToArray()))
            //        components.Add(comp);
            //}

            foreach (Component comp in childComps)
                if (CheckValidity(comp, validTypes.ToArray()))
                    components.Add(comp);


            if (!foldOutOpen)
            {
                if (script.aTargetType == null)
                    foldOutOpen = true;
            }

            foldOutOpen = EditorGUILayout.BeginFoldoutHeaderGroup(foldOutOpen, "Valid Types Selector [Optional]");
            if (foldOutOpen)
            {
                foreach (Component component in components)
                {
                    EditorGUILayout.BeginHorizontal();
                    GUI.contentColor = Color.green;
                    EditorGUILayout.SelectableLabel(component.gameObject.name + "\t[" + component.GetType().Name + "]");
                    GUI.contentColor = Color.white;
                    if (GUILayout.Button("Highlight"))
                    {
                        component.Ping();
                    }
                    if (GUILayout.Button("Select"))
                    {
                        serializedObject.FindProperty("aTargetType").objectReferenceValue = component;
                    }
                    EditorGUILayout.EndHorizontal();
                }
            }
            EditorGUILayout.EndFoldoutHeaderGroup();
        }



        serializedObject.ApplyModifiedProperties();
    }

    private bool CheckValidity(Component comp, params System.Type[] types)
    {
        System.Type compType = comp.GetType();
        foreach(System.Type type in types)
        {
            if (compType == type)
                return false;
        }
        return true;
    }

}

//Rect rect = EditorGUILayout.GetControlRect(true);
