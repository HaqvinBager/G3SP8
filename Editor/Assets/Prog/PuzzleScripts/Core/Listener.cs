using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public enum ListenerType
{
    Clear,
    Select,
    Rotate,
    Move,
    Toggle,
    Print,
}

public interface IListener
{
    void Remove();
}

[System.Serializable]
[ExecuteAlways]
public class Listener : MonoBehaviour
{
    public Lock myLock;
    public ListenerType responseType;

    [ExecuteAlways]
    void Awake()
    {
        responseType = ListenerType.Select;
    }

    [ExecuteAlways]
    private void Update()
    {
        if (responseType != ListenerType.Select)
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
                case ListenerType.Clear:
                    {
                        IListener[] others = GetComponents<IListener>();
                        foreach (IListener other in others)
                            other.Remove();
                    }
                    break;
            }
        }      
        responseType = ListenerType.Select;
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