using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResponsePrint : MonoBehaviour, IListener
{
    public string message = string.Empty;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}
