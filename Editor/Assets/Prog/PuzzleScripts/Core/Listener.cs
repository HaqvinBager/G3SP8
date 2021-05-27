using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public enum ListenerType
{
    None,
    Rotate,
    Move,
}

interface IListener
{
    void Remove();
}

[System.Serializable]
[ExecuteAlways]
public class Listener : MonoBehaviour
{
    public Lock myLock;
    public ListenerType responseType;
    private ListenerType old;

    [ExecuteAlways]
    private void Update()
    {
        if (old != responseType)
        {
            switch (responseType)
            {
                case ListenerType.Rotate:
                    if (GetComponent<Rotate>() == null)
                    {
#if UNITY_EDITOR
                        Undo.AddComponent(gameObject, typeof(Rotate));
#endif
                        break;
                    }
                    break;

                case ListenerType.Move:
                    if (GetComponent<Move>() == null)
                    {
#if UNITY_EDITOR
                        Undo.AddComponent(gameObject, typeof(Move));
#endif
                        break;
                    }
                    break;
                default:
                    {
                        IListener[] others = GetComponents<IListener>();
                        foreach (IListener other in others)
                            other.Remove();
                    }
                    break;
            }
        }
        old = responseType;
    }
}

