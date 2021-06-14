using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct PathPoint
{
    public Vector3 position;
    public Vector3 rotation;
    public float duration;
    //Optional data to add here
}

public class EndEvent : MonoBehaviour
{
    public Transform enemy;
    public float lockPlayerDuration = 5.0f;
    public List<PathPoint> path = new List<PathPoint>();
}
