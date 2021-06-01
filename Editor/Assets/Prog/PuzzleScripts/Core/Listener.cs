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
    Toggle,
    Print,
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
                    AddType<Rotate>(gameObject);
                    break;
                case ListenerType.Move:
                    AddType<Move>(gameObject);
                    break;
                case ListenerType.Print:
                    AddType<Print>(gameObject);
                    break;
                case ListenerType.Toggle:
                    AddType<Toggle>(gameObject);
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

    void AddType<T>(GameObject gameObject) where T : IListener
    {
        if (GetComponent<T>() == null)
        {
            #if UNITY_EDITOR
            Undo.AddComponent(gameObject, typeof(T));
            #endif
        }
    }
}