using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct EventListener
{
    public int instanceID;
    public string eventName;
}

[System.Serializable]
public struct IronEventCollection
{
    public List<EventListener> ironEvents;
}

public class ExportEventListener 
{
    public static IronEventCollection Export()
    {
        IronEventCollection collection = new IronEventCollection();
        collection.ironEvents = new List<EventListener>();
        IronEvent[] ironEvents = GameObject.FindObjectsOfType<IronEvent>();
        foreach(IronEvent ironEvent in ironEvents)
        {
            if (ironEvent.eventObject == null)
                continue;

            EventListener data = new EventListener();
            data.instanceID = ironEvent.transform.GetInstanceID();
            data.eventName = ironEvent.eventObject.name;
            collection.ironEvents.Add(data);
        }
        return collection;
    }
}