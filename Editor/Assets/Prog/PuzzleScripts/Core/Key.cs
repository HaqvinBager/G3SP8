using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public enum KeyType
{
    Clear,
    Select,
    Rotate,
    Move,
    PlayAudio,
    Destroy,
    Teleport
}

public interface IKey
{
    void Remove();
}

[System.Serializable]
[ExecuteAlways]
public class Key : MonoBehaviour
{
    public Lock myLock;
    public KeyType activationType;

    [ExecuteAlways]
    void Awake()
    {
        activationType = KeyType.Select;
    }

    [ExecuteAlways]
    private void Update()
    {
        if (activationType != KeyType.Select)
        {
            switch (activationType)
            {
                case KeyType.Rotate:
                    AddType<ActivationRotate>(gameObject);
                    break;
                case KeyType.Move:
                    AddType<ActivationMove>(gameObject);
                    break;
                case KeyType.PlayAudio:
                    AddType<ActivationPlayAudio>(gameObject);
                    break;
                case KeyType.Teleport:
                    AddType<ActivationTeleporter>(gameObject);
                    break;
                case KeyType.Clear:
                    {
                        IKey[] others = GetComponents<IKey>();
                        foreach (IKey other in others)
                            other.Remove();
                    }
                    break;
            }
        }
        activationType = KeyType.Select;
    }

    void AddType<T>(GameObject gameObject) where T : IKey
    {
        if (GetComponent<T>() == null)
        {
#if UNITY_EDITOR
            Undo.AddComponent(gameObject, typeof(T));
#endif
        }
    }
}

//public enum KeyInteractionType
//{
//    Destroy,
//    Animate,
//}

//public class Key : MonoBehaviour
//{
//    public Lock myLock;
//    public KeyInteractionType interactionType;
//}
