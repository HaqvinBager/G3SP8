using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class ExportIEvent 
{
    [MenuItem("Export/Events")]
    public static string Export(ExporterBin aExporter)
    {    
        List<IEvent> iEvents = MagicString.LoadAssets<IEvent>("t:ScriptableObject", "Assets/IEvents");
        
        Dictionary<IEvent, List<int>> eventMap = new Dictionary<IEvent, List<int>>();
        iEvents.ForEach(e => eventMap.Add(e, new List<int>()));
        
        var listeners = GameObject.FindObjectsOfType<IEventListener>();
        foreach (var listener in listeners)       
            if(eventMap.TryGetValue(listener?.myEvent, out List<int> list))
                list.Add(listener.transform.GetInstanceID());

        System.Text.StringBuilder log = new System.Text.StringBuilder();
        aExporter.Writer.Write(iEvents.Count);
        log.AppendLine("IEvent Count " + iEvents.Count);
        foreach (IEvent iEvent in iEvents)
        {
            if(eventMap.TryGetValue(iEvent, out List<int> list))
            {
                log.AppendLine(iEvent.name + " Listener Count " + list.Count);
                aExporter.Writer.Write(iEvent.GetInstanceID());
                aExporter.Writer.Write(list);
            }
        }
        return log.ToString();
    }
}


//[System.Serializable]
//public struct IEventData
//{
//    public int instanceID;
//    public List<int> listenerInstanceIDs;
//}

//[System.Serializable]
//public struct IEventCollection
//{
//    public List<IEventData> eventData;
//}