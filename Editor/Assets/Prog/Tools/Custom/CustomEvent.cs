using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName ="LD/Create Custom Event")]
public class CustomEvent : ScriptableObject
{
    private List<CustomEventListener> listeners = new List<CustomEventListener>();

    public void OnRaiseEvent()
    {
        foreach (var listener in listeners)
            listener.OnRespond();
    }

    public void Register(CustomEventListener aEvent)
    {
        if(!listeners.Contains(aEvent))
            listeners.Add(aEvent);
    }

    public void UnRegister(CustomEventListener aEvent)
    {
        if (listeners.Contains(aEvent))
            listeners.Remove(aEvent);
    }
}
