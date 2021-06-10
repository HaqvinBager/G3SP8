using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResponseNextLevel : MonoBehaviour, IListener
{
    public LevelCollection myTarget;
    public float myDelay = 0.001f;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }

    void OnDrawGizmos()
    {
    }
}