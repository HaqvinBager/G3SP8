using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

[System.Serializable]
public class ActivationRotate : MonoBehaviour, IKey
{
    [HideInInspector]
    public Vector3 start, end;
    public float duration = 0.5f;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}
