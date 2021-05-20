using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct FuseboxLink
{
    public int instanceID;
}

[System.Serializable]
public struct FuseboxCollection
{
    public List<FuseboxLink> myFuseboxes;
}

public class ExportFusebox
{
    public static FuseboxCollection Export(string aSceneName)
    {
        FuseboxCollection collection = new FuseboxCollection();
        collection.myFuseboxes = new List<FuseboxLink>();

        Fusebox[] allFuseboxes = GameObject.FindObjectsOfType<Fusebox>();
        foreach (Fusebox fusebox in allFuseboxes)
        {
            FuseboxLink fuseboxLink;
            fuseboxLink.instanceID = fusebox.transform.GetInstanceID();
            collection.myFuseboxes.Add(fuseboxLink);
        }

        return collection;
    }
}
