using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResponseAddForce : MonoBehaviour, IListener
{
    [Range(0.0f, 100.0f)]
    public float force;

    public Vector3 worldDirection = new Vector3(0.0f, 0.0f, 1.0f);


    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}
