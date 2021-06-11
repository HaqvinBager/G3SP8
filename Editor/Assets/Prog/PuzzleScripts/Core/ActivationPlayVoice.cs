using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ActivationPlayVoice : MonoBehaviour, IKey
{
    public EDialogueScene dialogueScene;
    public bool myIs3D;
    [Header("!Only Applicable for 3D sources!")]
    public Vector3 myConeDirection;
    public float myMinAttenuationAngle;
    public float myMaxAttenuationAngle;
    public float myMinAttenuationDistance = 1.0f;
    public float myMaxAttenuationDistance = 10000.0f;
    public float myMinimumVolume;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }

    void OnDrawGizmos()
    {
        Gizmos.DrawRay(transform.position, myConeDirection.normalized);
    }
}
