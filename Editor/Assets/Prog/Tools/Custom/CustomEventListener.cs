using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class CustomEventListener : MonoBehaviour
{
    public CustomEvent testEvent = null;

    [Space(50)]
    public UnityEvent onResponse = null;

    public List<CustomEvent> customResponse = new List<CustomEvent>();

    private void OnEnable()
    {
        testEvent?.Register(this);
    }

    private void OnDisable()
    {
        testEvent?.UnRegister(this);
    }

    public void OnRespond()
    {
        onResponse?.Invoke();
    }
}
