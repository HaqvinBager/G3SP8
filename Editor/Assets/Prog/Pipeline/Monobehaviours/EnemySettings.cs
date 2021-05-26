using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

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

    private NavMeshAgent myAgent;

    public PatrolPoint myTarget = null;

    private void Awake()
    {
        myAgent = gameObject.AddComponent<NavMeshAgent>();


        myTarget = FindBestPatrolPoint(transform.position);
    }

    private void Start()
    {
        myAgent.SetDestination(myTarget.transform.position);
    }

    private void Update()
    {
        PatrolPoint test = FindBestPatrolPoint(myTarget);
        Debug.DrawLine(transform.position, test.transform.position, Color.green);

        if (myTarget != null)
            Debug.DrawLine(transform.position, myTarget.transform.position, Color.cyan);

    }

    private void OnTriggerEnter(Collider other)
    {
        if(other.TryGetComponent(out PatrolPoint patrolPoint))
        {
            patrolPoint.AddBonusValue(10);
            myTarget = FindBestPatrolPoint(transform.position);
            myAgent.SetDestination(myTarget.transform.position);
        }
    }

    private PatrolPoint FindBestPatrolPoint(Vector3 position)
    {
        var patrols = FindObjectsOfType<PatrolPoint>();
        float lowestValue = float.MaxValue;

        int closestIndex = 0;
        for(int i = 0; i < patrols.Length; ++i)
        {
            if(lowestValue > patrols[i].GetInterestValue())
            {
                lowestValue = patrols[i].GetInterestValue();
                closestIndex = i;
            }
        }

        return patrols[closestIndex];
    }

    private PatrolPoint FindBestPatrolPoint(PatrolPoint exception)
    {
        var patrols = FindObjectsOfType<PatrolPoint>();
        float lowestValue = float.MaxValue;

        int closestIndex = 0;
        for (int i = 0; i < patrols.Length; ++i)
        {
            if (patrols[i] == exception)
                continue;

            if (lowestValue > patrols[i].GetInterestValue())
            {
                lowestValue = patrols[i].GetInterestValue();
                closestIndex = i;
            }
        }

        return patrols[closestIndex];
    }



    private void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        Gizmos.DrawWireSphere(transform.position, settings.radius);
    }
}
