﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct Enemy //Unity Interface for LD
{
    [HideInInspector]
    public int instanceID;

    public float speed;
    public float radius;
    public float health;
    public float attackDistance;
    public List<GameObject> points;
    [HideInInspector]
    public List<InterestPoint> interestPoints;
}

[System.Serializable]
public struct InterestPoint
{
  
    public float interestValue;
    public Transform transform;
}

public class EnemySettings : MonoBehaviour
{
    public Enemy settings = new Enemy { speed = 2.0f, radius = 10.0f,  health = 10.0f, attackDistance = 2.0f};
  
    

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        Gizmos.DrawWireSphere(transform.position, settings.radius);
    }
}
