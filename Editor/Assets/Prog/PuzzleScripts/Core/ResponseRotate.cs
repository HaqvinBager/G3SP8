using System.Collections;
using System.Collections.Generic;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

[System.Serializable]
public class ResponseRotate : MonoBehaviour, IListener
{
    [HideInInspector]
    public Vector3 start, end;
    public float duration;
    public float delay;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}

