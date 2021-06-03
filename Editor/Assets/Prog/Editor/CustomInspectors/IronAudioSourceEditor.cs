using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
[CustomEditor(typeof(IronAudioSource))]
public class IronAudioSourceEditor : Editor
{

    int selected = 0;
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        //List<string> options = new List<string>()
        //{
        //    "EMusic",
        //    "EAmbience",
        //    "EPropAmbience",
        //    "ESFX",
        //    "ESFXCollection",
        //    "EUI",
        //    "EResearcherEventVoiceLine",
        //    "EResearcherReactionVoiceLine",
        //    "EEnemyVoiceLine",
        //    "EChannel",
        //    "EGroundType"
        //};
        //selected = EditorGUILayout.Popup("Type: ", selected, options.ToArray());
    }

}
