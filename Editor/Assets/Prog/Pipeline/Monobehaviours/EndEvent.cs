using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct PathPoint
{
    public Vector3 position;
    public Quaternion rotation;
    public float duration;
    
    //Optional data to add here
    public float playerVingetteStrength;
}



public class EndEvent : MonoBehaviour
{
    public Transform enemy;
    public float lockPlayerDuration = 5.0f;
    public List<PathPoint> path = new List<PathPoint>();
    //public List<VFXPoint> vfx = new List<VFXPoint>();
}
