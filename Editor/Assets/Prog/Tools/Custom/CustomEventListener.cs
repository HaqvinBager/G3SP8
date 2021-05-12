using UnityEngine;
using UnityEngine.Events;

public class CustomEventListener : MonoBehaviour
{
    public CustomEvent testEvent = null;
    public UnityEvent onResponse = null;

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
