using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct ListenerData
{
    public string onResponseNotify;
    public int instanceID;
}

[System.Serializable]
public struct MoveData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public int instanceID;
}

[System.Serializable]
public struct RotateData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public int instanceID;
}

[System.Serializable]
public struct ListenerCollection
{
    public List<ListenerData> listeners;
    public List<MoveData> moves;
    public List<RotateData> rotates;
}

public class ExportResponse 
{  
    public static ListenerCollection Export()
    {
        ListenerCollection collection = new ListenerCollection();
        collection.listeners = new List<ListenerData>();
        collection.moves = new List<MoveData>();
        collection.rotates = new List<RotateData>();

        Listener[] listeners = GameObject.FindObjectsOfType<Listener>();
        foreach(Listener listener in listeners)
        {
            ListenerData data = new ListenerData();
            data.onResponseNotify = listener.myLock.onLockNotify.name;
            data.instanceID = listener.transform.GetInstanceID();
            collection.listeners.Add(data);

            ExportRotateResponses(collection, listener);
            ExportMoveResponses(collection, listener);
        }
        return collection;
    }

    private static void ExportMoveResponses(ListenerCollection collection, Listener response)
    {
        if (response.TryGetComponent(out Move move))
        {
            MoveData moveData = new MoveData();
            moveData.start = move.start;
            moveData.end = move.end;
            moveData.duration = move.duration;
            moveData.instanceID = move.transform.GetInstanceID();
            collection.moves.Add(moveData);
        }
    }

    private static void ExportRotateResponses(ListenerCollection collection, Listener response)
    {
        if (response.TryGetComponent(out Rotate rotate))
        {
            RotateData rotateData = new RotateData();
            rotateData.start = rotate.start;
            rotateData.end = rotate.end;
            rotateData.duration = rotate.duration;
            rotateData.instanceID = rotate.transform.GetInstanceID();
            collection.rotates.Add(rotateData);
        }
    }
}