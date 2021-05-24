using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Fusebox : MonoBehaviour
{
    public List<FusePickUp> myFuses = new List<FusePickUp>();

    private void OnDrawGizmosSelected()
    {
        FusePickUp[] fuses = FindObjectsOfType<FusePickUp>();
        foreach (var fuse in fuses)
        {
            Gizmos.DrawLine(transform.position, fuse.transform.position);
        }
    }
    private void OnDrawGizmos()
    {
        Gizmos.color = Gizmos.color * 0.5f;
        FusePickUp[] fuses = FindObjectsOfType<FusePickUp>();
        foreach (var fuse in fuses)
        {
            Gizmos.DrawLine(transform.position, fuse.transform.position);
        }

        foreach (FusePickUp fuse in fuses)
        {
            if (!myFuses.Contains(fuse))
                myFuses.Add(fuse);
        }

        foreach (FusePickUp fuse in myFuses)
            if (fuse == null)
                myFuses.Remove(fuse);
    }
}
