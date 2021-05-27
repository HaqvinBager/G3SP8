using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum KeyInteractionType
{
    Destroy,
    Animate,
}

public class Key : MonoBehaviour
{
    public Lock myLock;
    public KeyInteractionType interactionType;
}
