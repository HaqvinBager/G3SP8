using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct TeleporterData
{
    public Transform instanceID;
    public ELevelName   myTeleporterName;
    public ELevelName   teleportTo;
    public float teleportObjectToX;
    public float teleportObjectToY;
    public float teleportObjectToZ;
}

[System.Serializable]
public struct TeleporterCollection
{
    public List<TeleporterData> teleporters;
}

public class ExportLevelTeleporter
{
    public static TeleporterCollection Export(string aScene)
    {
        TeleporterCollection collection = new TeleporterCollection();
        collection.teleporters = new List<TeleporterData>();

        LevelTeleporter[] teleporters = GameObject.FindObjectsOfType<LevelTeleporter>();
        foreach (LevelTeleporter teleporter in teleporters)
        {
            collection.teleporters.Add(
                new TeleporterData
                {
                    instanceID = teleporter.transform,
                    myTeleporterName = teleporter.myTeleporterName,
                    teleportTo = teleporter.teleportTo,
                    teleportObjectToX = teleporter.onTeleportToMePlaceOnPosition.x,
                    teleportObjectToY = teleporter.onTeleportToMePlaceOnPosition.y,
                    teleportObjectToZ = teleporter.onTeleportToMePlaceOnPosition.z
                }
            );
        }

        return collection;
    }

}