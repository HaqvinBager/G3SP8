using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum IronEventType
{
    //SoundEffectEvent,
    GameEvent,
    Move,
    //VFXEvent
}

public enum EEventFilter // Needs to be 1:1 in C++
{
    PlayerOnly,
    EnemyOnly,
    ObjectOnly,
    Any
}

public class IronEvent : MonoBehaviour
{
    public IronEventType eventType;
    public GameEvent eventObject;
    public EEventFilter eventFilter = EEventFilter.Any;
    public EDialogueScene dialogueScene = EDialogueScene.Count;
    public bool triggerOnce = false;
}

public class IronMoveEvent : IronEvent
{
    public Vector3 start;
    public Vector3 end;
    public float moveSpeed;
}