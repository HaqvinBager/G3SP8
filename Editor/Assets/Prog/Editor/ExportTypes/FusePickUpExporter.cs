using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct FusePickUpLink
{
    public int instanceID;
}

[System.Serializable]
public struct FusePickUpCollection
{
    public List<FusePickUpLink> myFusePickUps;
}

public class FusePickUpExporter
{
    public static FusePickUpCollection Export(string aSceneName)
    {
        FusePickUpCollection collection = new FusePickUpCollection();
        collection.myFusePickUps = new List<FusePickUpLink>();

        FusePickUp[] allFusePickups = GameObject.FindObjectsOfType<FusePickUp>();
        foreach (FusePickUp fuse in allFusePickups)
        {
            FusePickUpLink fusePickupLink;
            fusePickupLink.instanceID = fuse.transform.GetInstanceID();
            collection.myFusePickUps.Add(fusePickupLink);
        }

        return collection;
    }
}
