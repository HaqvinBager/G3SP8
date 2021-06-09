using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResponsePlayAudio : MonoBehaviour, IListener
{
    public ESFX soundEffect;
    public bool myIs3D;
    [Header("!Only Applicable for 3D sources!")]
    public Vector3 myConeDirection;
    public float myMinAttenuationAngle;
    public float myMaxAttenuationAngle;
    public float myMinAttenuationDistance = 1.0f;
    public float myMaxAttenuationDistance = 10000.0f;
    public float myMinimumVolume;
    public float myDelay;

    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }

    void OnDrawGizmos()
    {
        Gizmos.DrawRay(transform.position, myConeDirection.normalized);


        //Vector3 dir = myConeDirection + new Vector3(Mathf.Cos(myMinAttenuationAngle * Mathf.Deg2Rad), 0.0f, Mathf.Sin(myMinAttenuationAngle * Mathf.Deg2Rad));
        //Gizmos.color = Color.red * 0.75f;
        //Gizmos.DrawRay(transform.position, newDir.normalized);
        //Vector3 otherDir = myConeDirection + new Vector3(Mathf.Cos(myMinAttenuationAngle * Mathf.Deg2Rad), 0.0f, Mathf.Sin(myMinAttenuationAngle * Mathf.Deg2Rad));

        //Gizmos.color = Color.blue * 0.5f;
        //Gizmos.DrawRay(transform.position, otherDir.normalized);
    }


    //void OnDrawGizmosSelected()
    //{
    //    Gizmos.DrawRay(transform.position, myConeDirection.normalized);
 
    //    Vector3 dir = myConeDirection + new Vector3(Mathf.Sin(myMinAttenuationAngle * Mathf.Deg2Rad), 0.0f, Mathf.Sin(myMinAttenuationAngle * Mathf.Deg2Rad));
    //    Gizmos.color = Color.red;
    //    Gizmos.DrawRay(transform.position, dir.normalized);
    //    //Vector3 otherDir = myConeDirection + new Vector3(Mathf.Sin(myMinAttenuationAngle * Mathf.Deg2Rad), 0.0f, Mathf.Sin(myMinAttenuationAngle * Mathf.Deg2Rad));

    //    //Gizmos.color = Color.blue;
    //    //Gizmos.DrawRay(transform.position, otherDir.normalized);
    //}

}
