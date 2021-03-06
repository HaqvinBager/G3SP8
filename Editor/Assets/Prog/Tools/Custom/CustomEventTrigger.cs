using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

[RequireComponent(typeof(BoxCollider))]
public class CustomEventTrigger : MonoBehaviour
{
    public CustomEvent triggerEvent = null;

    [Space(50)]
    public UnityEvent onTriggerActivation = new UnityEvent();

    public void OnValidate()
    {
        if (TryGetComponent(out Collider collider))
            collider.isTrigger = true;
    }

    private void OnTriggerEnter(Collider other)
    {
        if(triggerEvent != null)
            triggerEvent.OnRaiseEvent();

        onTriggerActivation?.Invoke();
        Debug.DrawLine(other.transform.position, transform.position, Color.green * 0.75f, 5.5f);
    }
}
