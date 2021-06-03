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
    PlayAudio,
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
                    AddType<ResponseRotate>(gameObject);
                    break;
                case ListenerType.Move:
                    AddType<ResponseMove>(gameObject);
                    break;
                case ListenerType.Print:
                    AddType<ResponsePrint>(gameObject);
                    break;
                case ListenerType.Toggle:
                    AddType<ResponseToggle>(gameObject);
                    break;
                case ListenerType.PlayAudio:
                    AddType<ResponsePlayAudio>(gameObject);
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