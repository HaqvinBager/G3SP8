using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct VFXLink
{
    public int instanceID;
    public string effectName;
}

[System.Serializable]
public struct VFXCollection
{
    public List<VFXLink> myVFXLinks;
}

public class ExportVFX
{
    public static VFXCollection Export(string aSceneName)
    {
        VFXCollection collection = new VFXCollection();
        collection.myVFXLinks = new List<VFXLink>();

        VFXReference[] allVFXRefs = GameObject.FindObjectsOfType<VFXReference>();
        foreach (VFXReference vfxRef in allVFXRefs)
        {
            VFXLink vfxLink;
            vfxLink.instanceID = vfxRef.transform.GetInstanceID();
            vfxLink.effectName = vfxRef.effectName;
            collection.myVFXLinks.Add(vfxLink);
        }

        return collection;
    }
}
