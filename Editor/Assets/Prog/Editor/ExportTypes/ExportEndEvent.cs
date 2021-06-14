using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[System.Serializable]
public struct EndEventData
{
    public List<PathPoint> path;
    public int enemyInstanceID;
    public int playerInstanceID;
    public int instanceID;
    public float lockPlayerDuration;
}

[System.Serializable]
public struct EndEventCollection
{
    public EndEventData endEvent;
}

public class ExportEndEvent 
{
    public static bool TryExport(PlayerID playerData, out EndEventCollection outData)
    {
        EndEvent endEvent = GameObject.FindObjectOfType<EndEvent>();
        outData.endEvent = new EndEventData();
        outData.endEvent.path = new List<PathPoint>();
        if (endEvent != null)
        {
            outData.endEvent.enemyInstanceID = endEvent.enemy.GetInstanceID();
            outData.endEvent.playerInstanceID = playerData.instanceID;
            outData.endEvent.instanceID = endEvent.transform.GetInstanceID();
            outData.endEvent.lockPlayerDuration = endEvent.lockPlayerDuration;
            foreach (PathPoint point in endEvent.path)
            {
                outData.endEvent.path.Add(point);
            }

            return true;
        }
        return false;
    }

    //public static EndEventCollection Export(PlayerID playerData)
    //{
    //    EndEventCollection data = new EndEventCollection();
    //    data.endEvent = new EndEventData();
    //    EndEvent endEvent = GameObject.FindObjectOfType<EndEvent>();
    //    if(endEvent != null)
    //    {
    //        data.endEvent.enemyInstanceID = endEvent.enemy.GetInstanceID();
    //        data.endEvent.playerInstanceID = playerData.instanceID;
    //        data.endEvent.instanceID = endEvent.transform.GetInstanceID();
    //        foreach(PathPoint point in endEvent.path)
    //        {
    //            data.endEvent.path.Add(point);
    //        }
    //    }
    //    return data;
    //}
}
