using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class ActivationLight : MonoBehaviour, IKey
{
    public Color setColor;
    public float setIntensity = 1.0f;
    public Transform target;
    public Light targetLight;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}
