using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct SafetyDoorLink
{
    public int instanceID;
}

[System.Serializable]
public struct SafetyDoorCollection
{
    public List<SafetyDoorLink> mySafetyDoors;
}

public class ExportSafetyDoors
{
    public static SafetyDoorCollection Export(string aSceneName)
    {
        SafetyDoorCollection collection = new SafetyDoorCollection();
        collection.mySafetyDoors = new List<SafetyDoorLink>();

        SafetyDoor[] allSafetyDoors = GameObject.FindObjectsOfType<SafetyDoor>();
        foreach (SafetyDoor safetyDoor in allSafetyDoors)
        {
            SafetyDoorLink safetyDoorLink;
            safetyDoorLink.instanceID = safetyDoor.transform.GetInstanceID();
            collection.mySafetyDoors.Add(safetyDoorLink);
        }

        return collection;
    }
}