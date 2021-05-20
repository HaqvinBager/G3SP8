using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class SelectVP : ScriptableWizard
{
    string tagName = "VP";

    [MenuItem("Tools/Select All VertexPaint Objects #g")]
    static void SelectAllOfTagWizard()
    {
        ScriptableWizard.DisplayWizard("Select ALL Vertex Paint Meshes to Edit!", typeof(SelectVP), "Select! <3");
    }

    void OnWizardCreate()
    {
        GameObject[] gos = GameObject.FindGameObjectsWithTag(tagName);
        List<GameObject> vpGos = new List<GameObject>();
        foreach (GameObject go in gos)
        {
            if (go.GetComponentInChildren<PolybrushFBX>())
            {
                vpGos.Add(go.transform.GetChild(0).gameObject);
            }
        }
        Selection.objects = vpGos.ToArray();
    }
}
