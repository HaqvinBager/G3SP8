using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Print : MonoBehaviour, IListener
{
    public string message = string.Empty;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}
