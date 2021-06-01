using System;
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
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct RotateData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct PrintData
{
    public string data;
    public int instanceID;
}

[System.Serializable]
public struct ToggleData
{
    public bool data;
    public string aType;
    public int targetInstanceID;
    public int instanceID;
}

[System.Serializable]
public struct ListenerCollection
{
    public List<ListenerData> listeners;
    public List<MoveData> moves;
    public List<RotateData> rotates;
    public List<PrintData> prints;
}

public class ExportResponse 
{  
    public static ListenerCollection Export()
    {
        ListenerCollection collection = new ListenerCollection();
        collection.listeners = new List<ListenerData>();
        collection.moves = new List<MoveData>();
        collection.rotates = new List<RotateData>();
        collection.prints = new List<PrintData>();

        Listener[] listeners = GameObject.FindObjectsOfType<Listener>();
        foreach(Listener listener in listeners)
        {
            ListenerData data = new ListenerData();
            data.onResponseNotify = listener.myLock?.onLockNotify?.name;
            data.instanceID = listener.transform.GetInstanceID();
            collection.listeners.Add(data);

            ExportRotateResponses(ref collection.rotates, listener);
            ExportMoveResponses(ref collection.moves, listener);
            ExportPrintResponses(ref collection.prints, listener);
        }
        return collection;
    }

    private static void ExportPrintResponses(ref List<PrintData> prints, Listener listener)
    {
        if (listener.TryGetComponent(out Print obj))
        {
            PrintData data = new PrintData();
            data.instanceID = obj.transform.GetInstanceID();
            data.data = obj.message;
            prints.Add(data);
        }
    }

    private static void ExportMoveResponses(ref List<MoveData> moves, Listener listener)
    {
        if (listener.TryGetComponent(out Move move))
        {
            MoveData moveData = new MoveData();
            moveData.start = move.start;
            moveData.end = move.end;
            moveData.duration = move.duration;
            moveData.delay = move.delay;
            moveData.instanceID = move.transform.GetInstanceID();
            moves.Add(moveData);
        }
    }

    private static void ExportRotateResponses(ref List<RotateData> collection, Listener response)
    {
        if (response.TryGetComponent(out Rotate rotate))
        {
            RotateData rotateData = new RotateData();
            rotateData.start = rotate.start;
            rotateData.end = rotate.end;
            rotateData.duration = rotate.duration;
            rotateData.delay = rotate.delay;
            rotateData.instanceID = rotate.transform.GetInstanceID();
            collection.Add(rotateData);
        }
    }
}