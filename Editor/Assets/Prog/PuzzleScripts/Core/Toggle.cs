using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif


public class Toggle : MonoBehaviour, IListener
{
    public bool enabledOnStart = true;
    public bool enableOnNotify = false;
    public Transform aTarget = null;
    public Component aTargetType = null;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}
