using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class IronAudioSource : MonoBehaviour
{
    //public EPropAmbience sound;
    public int mySoundIndex;
    public bool myIs3D;
    [Header("!Only Applicable for 3D sources!")]
    public Vector3 myConeDirection;
    public float myMinAttenuationAngle;
    public float myMaxAttenuationAngle;
    public float myMinAttenuationDistance = 1.0f;
    public float myMaxAttenuationDistance = 10000.0f;
    public float myMinimumVolume;

}
