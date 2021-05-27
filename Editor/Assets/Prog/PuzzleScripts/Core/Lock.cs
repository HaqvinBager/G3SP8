using UnityEngine;

public enum LockInteractionType
{
    OnTriggerEnter,
    OnLeftClickDown,
}

public class Lock : MonoBehaviour
{
    public CustomEvent onKeyCreateNotify;
    public CustomEvent onKeyInteractNotify;
    public CustomEvent onLockNotify;
    public LockInteractionType interactionType;
}