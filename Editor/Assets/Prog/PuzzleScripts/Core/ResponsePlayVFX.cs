using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

[System.Serializable]
public class ResponsePlayVFX : MonoBehaviour, IListener
{
    public float duration = 0.5f;
    public float delay;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}