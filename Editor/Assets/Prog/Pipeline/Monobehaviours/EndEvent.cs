using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public enum EEnemyAnimation
{
    Alert = 1,
    Attack,
    Chase,// == Seek
    Grab,
    Idle,
    Walk
  //  Count
};

/*
 *     Alert = 4,
    Attack = 5,
    Chase = 1,// == Seek
    Grab = 6,
    Idle = 3,
    Walk = 2
 */

[System.Serializable]
public struct PathPoint
{
    public Vector3 position;
    public Quaternion rotation;
    public float duration;
    
    //Optional data to add here
    public float playerVingetteStrength;
    public EEnemyAnimation animation;
}



public class EndEvent : MonoBehaviour
{
    public Transform enemy;
    public float lockPlayerDuration = 5.0f;
    public List<PathPoint> path = new List<PathPoint>();
    //public List<VFXPoint> vfx = new List<VFXPoint>();
}
