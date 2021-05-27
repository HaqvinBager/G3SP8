using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public enum ResponseType
{
    None,
    Rotate,
    Move,
}

interface IResponse
{
    void Remove();
}

[System.Serializable]
[ExecuteAlways]
public class Response : MonoBehaviour
{
    public Lock myLock;
    public ResponseType responseType;

    private ResponseType old;

    [ExecuteAlways]
    private void Update()
    {
        if (old != responseType)
        {
            switch (responseType)
            {
                case ResponseType.Rotate:
                    if (GetComponent<ResponseRotate>() == null)
                    {
#if UNITY_EDITOR
                        Undo.AddComponent(gameObject, typeof(ResponseRotate));
#endif
                        break;
                    }
                    break;

                case ResponseType.Move:
                    if (GetComponent<ResponseMove>() == null)
                    {
#if UNITY_EDITOR
                        Undo.AddComponent(gameObject, typeof(ResponseMove));
#endif
                        break;
                    }
                    break;
                default:
                    {
                        IResponse[] others = GetComponents<IResponse>();
                        foreach (IResponse other in others)
                            other.Remove();
                    }
                    break;
            }
        }
        old = responseType;
    }
}

