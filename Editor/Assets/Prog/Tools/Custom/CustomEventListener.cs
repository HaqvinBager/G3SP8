using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class CustomEventListener : MonoBehaviour
{
    public CustomEvent testEvent = null;

    [Space(50)]
    public UnityEvent onActivation = null;

    public List<CustomEvent> customActivation = new List<CustomEvent>();

    private void OnEnable()
    {
        if(testEvent != null)
            testEvent.Register(this);
    }

    private void OnDisable()
    {
        if(testEvent != null)
            testEvent.UnRegister(this);
    }

    public void OnRespond()
    {
        if(onActivation != null)
            onActivation.Invoke();
    }
}
