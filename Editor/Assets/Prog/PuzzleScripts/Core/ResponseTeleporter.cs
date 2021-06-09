using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResponseTeleporter : MonoBehaviour, IListener
{
    public ELevelName myName;
    public ELevelName myTarget;
    public float myTeleportTimer;

    [HideInInspector]
    public Vector3 onTeleportToMePosition;
    [HideInInspector]
    public Vector3 onTeleportToMeRotation;

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

