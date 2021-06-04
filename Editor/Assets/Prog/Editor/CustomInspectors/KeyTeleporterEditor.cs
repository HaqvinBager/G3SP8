using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

[CustomEditor(typeof(ActivationTeleporter))]
public class KeyTeleporterEditor : Editor
{
    public override void OnInspectorGUI()
    {
        serializedObject.Update();
        base.OnInspectorGUI();

        this.ShowValueAndButton<ActivationTeleporter>("onTeleportToMePosition", "Teleport to Position", SavePosition);
        this.ShowValueAndButton<ActivationTeleporter>("onTeleportToMeRotation", "Rotate to", SaveRotation);

        GUI.backgroundColor = Color.green;
        this.SetValueButtonCallback<ActivationTeleporter>("onTeleportToMePosition", "Move To: Teleport to", SetPosition);
        GUI.backgroundColor = Color.cyan;
        this.SetValueButtonCallback<ActivationTeleporter>("onTeleportToMeRotation", "Rotate to: Rotate to", SetRotation);

        serializedObject.ApplyModifiedProperties();
    }

     void SetPosition(Vector3 aValue)
     {
         var move = (ActivationTeleporter)target;
         move.transform.position = aValue;
     }
     void SavePosition(SerializedProperty aProperty, Vector3 aValue)
     {
         aProperty.vector3Value = aValue;
     }

    private void SetRotation(Quaternion quat)
    {
        var rotate = (ActivationTeleporter)target;
        rotate.transform.rotation = quat;
    }
    private void SaveRotation(SerializedProperty aProperty, Quaternion aValue)
    {
        aProperty.vector3Value = aValue.eulerAngles;
    }
}
