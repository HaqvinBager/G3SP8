using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

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
    public string type;
    public int enableOnStartup;
    public int enableOnNotify;
    public int target;
    public int instanceID;
}

[System.Serializable]
public struct ListenerCollection
{
    public List<ListenerData> listeners;
    public List<MoveData> moves;
    public List<RotateData> rotates;
    public List<PrintData> prints;
    public List<ToggleData> toggles;
}

public class ExportResponse 
{  
    public static ListenerCollection Export()
    {
        ListenerCollection collection = new ListenerCollection();
        collection.listeners = new List<ListenerData>();
        collection.moves = new List<MoveData>();
        collection.rotates = new List<RotateData>();
        collection.toggles = new List<ToggleData>();
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
            ExportToggleResponses(ref collection.toggles, listener);
            ExportPrintResponses(ref collection.prints, listener);
        }
        return collection;
    }

    private static void ExportToggleResponses(ref List<ToggleData> toggles, Listener listener)
    {
        if (listener.TryGetComponent(out Toggle obj))
        {
            ToggleData data = new ToggleData();
            data.instanceID = obj.transform.GetInstanceID();
            data.type = obj.aTargetType.GetType().Name;
            if (obj.aTarget == null)
                Debug.Log("Missing Reference", obj);
            data.target = obj.aTarget.transform.GetInstanceID();
            data.enableOnStartup = obj.enabledOnStart ? 1 : 0;
            data.enableOnNotify = obj.enableOnNotify ? 1 : 0;
            toggles.Add(data);
        }
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