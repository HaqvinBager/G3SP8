using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[System.Serializable]
public struct EventListener
{
    public int instanceID;
    public string eventName;
}

[System.Serializable]
public struct IronEventCollection
{
    public List<EventListener> collection;
}

public class ExportEventListener 
{

    public static IronEventCollection Export()
    {
        IronEventCollection collection = new IronEventCollection();
        collection.collection = new List<EventListener>();
        IronEvent[] ironEvents = GameObject.FindObjectsOfType<IronEvent>();
        foreach(IronEvent ironEvent in ironEvents)
        {
            EventListener data = new EventListener();
            data.instanceID = ironEvent.transform.GetInstanceID();
            data.eventName = ironEvent.eventObject.name;
            collection.collection.Add(data);
        }
        return collection;
    }
}
